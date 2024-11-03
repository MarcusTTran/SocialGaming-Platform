#include "RulesRunner.h"

void RulesRunner::configurationSetter(const Configuration& config) {
    this->configuration = config;
}
void RulesRunner::variablesSetter(const DataValue::OrderedMapType& vars) {
    variables = vars;
}
void RulesRunner::constantsSetter(const DataValue::OrderedMapType& consts) {
    constants = consts;
}
void RulesRunner::perPlayerSetter(const DataValue::OrderedMapType& perPlayers) {
    perPlayer = perPlayers;
}
void RulesRunner::perAudienceSetter(const DataValue::OrderedMapType& perAudiences) {
    perAudience = perAudiences;
}
void RulesRunner::rulesSetter(const std::vector<Rule>& r) {
    rules = r;
}
DataValue::OrderedMapType RulesRunner::constantsGetter(){
    return constants;
}


void RulesRunner::processRule(const std::vector<Rule>& rules, std::unordered_map<std::string, std::string> &loopVariables){
    auto processRule = [&](const Rule& rule, const auto& self) -> void {
        std::for_each(rule.subRules.begin(), rule.subRules.end(), [&](const Rule& subRule) {
            self(subRule, self); // Recursive call
        });
        if(rule.type == Rule::Type::For){
            implementForRule(rule, loopVariables);
        }
        if (rule.type == Rule::Type::Message) {
            std::string key = extractPlaceholders(rule);
            printMessageWithPlaceholders(rule, key, loopVariables);
        }
    };

    std::for_each(rules.begin(), rules.end(), [&](const Rule& rule) {
        processRule(rule, processRule);
    });
}

std::string RulesRunner::extractPlaceholders(const Rule& rule) {
    std::string key;
    for (const auto& param : rule.parameters) {
        if (std::holds_alternative<std::string>(param)) {
            const std::string& str = std::get<std::string>(param);
            // Check if the string is a placeholder name (e.g., "round")
            // TODO: a better way to find key
            if (str == "round") {  
                key = str;
                break;  
            }
        }
    }
    return key;  
}


void RulesRunner::printMessageWithPlaceholders(const Rule& rule, const std::string& key, const std::unordered_map<std::string, std::string>& loopVariables) {
    std::string messageTemplate;

    for (const auto& param : rule.parameters) {
        if (std::holds_alternative<std::string>(param)) {
            const std::string& str = std::get<std::string>(param);

            // Skip unnecessary tokens
            // Skip all for now. TODO: need to add all to notice all players
            if (str == "message" || str == "all" || str == "\"" || str == ";" || str == "{" || str == "}") {
                continue;
            }

            // Use placeholder format for the loop variable key
            if (str == key) {
                messageTemplate += "{" + key + "}";
            } else {
                messageTemplate += str + " ";
            }
        }
    }

    if (loopVariables.find(key) == loopVariables.end()) {
        return;
    }

    // Replace placeholder `{key}` with `loopVariables[key]`
    std::string placeholder = "{" + key + "}";
    std::string outputMessage;
    size_t pos = 0;
    size_t lastPos = 0;

    // Search for placeholders and replace them
    while ((pos = messageTemplate.find(placeholder, lastPos)) != std::string::npos) {
        outputMessage += messageTemplate.substr(lastPos, pos - lastPos); // Add text before placeholder
        outputMessage += loopVariables.at(key); // Replace placeholder with actual value
        lastPos = pos + placeholder.length(); // Move past the placeholder
    }

    outputMessage += messageTemplate.substr(lastPos);
    std::cout << outputMessage << std::endl;
}




std::string RulesRunner::ruleTypeToString(Rule::Type type){
    switch (type) {
        case Rule::Type::For: return "For";
        case Rule::Type::Loop: return "Loop";
        case Rule::Type::ParallelFor: return "ParallelFor";
        case Rule::Type::InParallel: return "InParallel";
        case Rule::Type::Match: return "Match";
        case Rule::Type::Extend: return "Extend";
        case Rule::Type::Reverse: return "Reverse";
        case Rule::Type::Shuffle: return "Shuffle";
        case Rule::Type::Sort: return "Sort";
        case Rule::Type::Deal: return "Deal";
        case Rule::Type::Discard: return "Discard";
        case Rule::Type::Timer: return "Timer";
        case Rule::Type::InputChoice: return "InputChoice";
        case Rule::Type::InputText: return "InputText";
        case Rule::Type::InputVote: return "InputVote";
        case Rule::Type::InputRange: return "InputRange";
        case Rule::Type::Message: return "Message";
        case Rule::Type::Scores: return "Scores";
        case Rule::Type::Assignment: return "Assignment";
        case Rule::Type::Body: return "Body";
        default: return "Unknown";
    }
}

void RulesRunner::implementForRule(const Rule& rule, std::unordered_map<std::string, std::string> &loopVariables) {
    if (rule.parameters.size() <= 1) {
        std::cerr << "Insufficient parameters for 'For' rule." << std::endl;
        return;
    }

    // Extract loop variable
    auto loopVariable = std::get_if<std::string>(&rule.parameters[0]);
    if (!loopVariable) {
        std::cerr << "Invalid loop variable in 'For' rule." << std::endl;
        return;
    }

    // Extract target path and built-in function
    std::vector<std::string> targetPath;
    std::optional<std::string> builtinFunction;
    auto it = std::find_if(rule.parameters.begin() + 1, rule.parameters.end(), [&](const auto& param) {
        if (auto str = std::get_if<std::string>(&param); str && std::find(GameConstantsType::builtin.begin(), GameConstantsType::builtin.end(), *str) != GameConstantsType::builtin.end()) {
            builtinFunction = *str;
            return true;
        }
        if (auto str = std::get_if<std::string>(&param)) {
            targetPath.push_back(*str);
        }
        return false;
    });

    if (targetPath.empty()) {
        std::cerr << "Target path is empty." << std::endl;
        return;
    }

    // Determine initial container
    // targetPath = {"configuration", "rounds", "upfrom", 1}
    std::shared_ptr<void> currentContainer;
    const auto& rootKey = targetPath[0];
    if (rootKey == "configuration") currentContainer = std::make_shared<decltype(configuration.setup)>(configuration.setup);
    else if (rootKey == "constants") currentContainer = std::make_shared<decltype(constants)>(constants);
    else if (rootKey == "variables") currentContainer = std::make_shared<decltype(variables)>(variables);
    else if (rootKey == "perPlayer") currentContainer = std::make_shared<decltype(perPlayer)>(perPlayer);
    else if (rootKey == "perAudience") currentContainer = std::make_shared<decltype(perAudience)>(perAudience);
    else {
        std::cerr << "Unknown target path root: " << rootKey << std::endl;
        return;
    }

    // Traverse setup container to find entry
    auto setupContainer = std::static_pointer_cast<decltype(configuration.setup)>(currentContainer);
    std::optional<std::vector<std::map<std::string, std::string>>> entryOpt;

    if (setupContainer && targetPath.size() > 1) {
        const auto& target = targetPath[1];   // rounds
        entryOpt = std::find_if(setupContainer->begin(), setupContainer->end(),
            [&target](const auto& setupEntry) {
                return setupEntry.count(target) > 0;
            })->at(target);
    }

    if (!entryOpt) {
        std::cerr << "Failed to locate path " << targetPath.back() << " within " << rootKey << std::endl;
        return;
    }

    // Debugging output
    // std::cout << "Contents of '" << targetPath[1] << "' entry:" << std::endl;
    // for (const auto& mapEntry : *entryOpt) {
    //     for (const auto& [key, value] : mapEntry) {
    //         std::cout << "Key: " << key << ", Value: " << value << std::endl;
    //     }
    // }

    // Process built-in function like `upfrom`
    if (builtinFunction && *builtinFunction == "upfrom") {
        int start = std::get<int>(rule.parameters.back());
        int end = -1;

        auto rangeString = std::ranges::find_if(*entryOpt, [](const auto& mapEntry) {
            return mapEntry.count("range") > 0;
        })->at("range");

        if (!rangeString.empty()) {
            size_t commaPos = rangeString.find(',');
            if (commaPos != std::string::npos) end = std::stoi(rangeString.substr(commaPos + 1));
        }

        if (end != -1) {
            std::cout << "Looping from " << start << " to " << end << " for " << *loopVariable << std::endl;
            for (size_t i = start; i <= end; ++i) {
                loopVariables[*loopVariable] = std::to_string(i);
                if(!rule.subRules.empty()){
                    processRule(rule.subRules, loopVariables);
                }
            }
        } else {
            std::cerr << "'range' not found in " << targetPath.back() << " entry." << std::endl;
        }
    }
}

void RulesRunner::printRule(const Rule& rule, int indent){
    // Create indentation string based on the current level
    std::string indentStr(indent, ' ');

    // Lambda to print a std::variant based on its type
    auto visitor = [&](const auto& value) {
        using T = std::decay_t<decltype(value)>; // Get the underlying type

        // Check the type and print accordingly
        if constexpr (std::is_same_v<T, int>) {
            std::cout << indentStr << "1. int: " << value << std::endl;
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << indentStr << "2. string: " << value << std::endl;
        } else if constexpr (std::is_same_v<T, bool>) {
            std::cout << indentStr << "3. bool: " << (value ? "true" : "false") << std::endl;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (!value.empty()) {
                std::cout << indentStr << "4. vector<string>: [";
                for (const auto& str : value) {
                    std::cout << str << " ";
                }
                std::cout << "]" << std::endl;
            }
        }
    };

    // Print the rule type only if meaningful parameters exist
    bool hasParameters = !rule.parameters.empty();
    if (hasParameters) {
        std::cout << indentStr << "Main Rule:" << std::endl;
        std::cout << indentStr << "Rule Type: " << ruleTypeToString(rule.type) << std::endl;

        // Print the parameters of the rule
        std::cout << indentStr << "Parameters:" << std::endl;
        for (const auto& param : rule.parameters) {
            std::visit(visitor, param); // Use std::visit to apply the visitor to the variant
        }
    }

    // Recursively print subrules, with increased indentation
    if (!rule.subRules.empty()) {
        // std::cout << indentStr << "Sub Rules:" << std::endl;
        for (const auto& subRule : rule.subRules) {
            printRule(subRule, indent + 2); // Increase indentation for subrules
        }
    }
}

void RulesRunner::processRules(){
    std::for_each(begin(rules), end(rules), [this](const Rule& rule) {
    this->printRule(rule, 0);});

    // sth like index, round
    std::unordered_map<std::string, std::string> loopVariables;
    processRule(rules, loopVariables);
}