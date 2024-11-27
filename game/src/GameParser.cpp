// author: kwa132, Mtt8

#include "GameParser.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

extern "C" {
TSLanguage *tree_sitter_socialgaming();
}


ParsedGameData::ParsedGameData(const string &config, std::shared_ptr<Messenger> &server) : server(server) {
    string fileContent = readFileContent(config);
    if (!fileContent.empty()) {
        parseConfig(fileContent);
    }
}
string ParsedGameData::readFileContent(const string &filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

string ParsedGameData::getGameName() const { return configuration.name; }

pair<int, int> ParsedGameData::getPlayerRange() const { return configuration.range; }

bool ParsedGameData::hasAudience() const { return configuration.audience; }

Configuration ParsedGameData::getConfiguration() { return configuration; }

const DataValue::OrderedMapType &ParsedGameData::getConstants() const { return constants; }

const DataValue::OrderedMapType &ParsedGameData::getVariables() const { return variables; }

const DataValue::OrderedMapType &ParsedGameData::getPerPlayer() const { return perPlayer; }

const DataValue::OrderedMapType &ParsedGameData::getPerAudience() const { return perAudience; }

const std::vector<DataValue::OrderedMapType> &ParsedGameData::getSetup() const { return configuration.setup; }

const vector<std::unique_ptr<Rule>>& ParsedGameData::getRules() const { return rules; }

DataValue ParsedGameData::handleExpression(const ts::Node &node, const std::string &source) {
    auto type = node.getType();
    auto currContent = std::string(node.getSourceRange(source));

    if (std::find(GameConstantsType::toSkip.begin(), GameConstantsType::toSkip.end(), currContent) !=
        GameConstantsType::toSkip.end()) {
        return DataValue("");
    }

    if (type == "boolean") {
        bool curr = (currContent == "true");
        return DataValue(curr);
    } else if (type == "number") {
        int curr = std::stoi(currContent);
        return DataValue(curr);
    } else if (type == "number_range") {
        size_t start = currContent.find('(');
        size_t comma = currContent.find(',');
        size_t end = currContent.find(')');
        if (start != std::string::npos && comma != std::string::npos && end != std::string::npos) {
            try {
                int minRange = std::stoi(currContent.substr(start + 1, comma - start - 1));
                int maxRange = std::stoi(currContent.substr(comma + 1, end - comma - 1));
                return DataValue(std::make_pair(minRange, maxRange));
            } catch (const std::exception &e) {
                std::cerr << "Error parsing number_range: " << e.what() << std::endl;
                return DataValue("");
            }
        }
        return DataValue("");
    } else if (type == "quoted_string" || type == "string_text" || type == "identifier") {
        std::string curr = currContent;
        if (curr.length() > 2 && type == "quoted_string") {
            curr = curr.substr(1, curr.length() - 2); // Remove quotes
        }
        return DataValue(curr);
    } else if (type == "list_literal") {
        std::vector<DataValue> list;
        for (const auto &child : ts::Children{node.getChild(1)}) {
            DataValue item = handleExpression(child, source);
            if (!std::holds_alternative<std::string>(item.getValue()) ||
                !std::get<std::string>(item.getValue()).empty()) {
                list.emplace_back(std::move(item));
            }
        }
        return DataValue(std::move(list));
    } else if (type == "value_map") {
        DataValue::OrderedMapType subContent;
        parseValueMap(node, source, subContent);
        return DataValue(std::move(subContent));
    } else if (type == "enum_description") {
        DataValue::EnumDescriptionType enumVector;
        auto keyNode = node.getChildByFieldName("name");
        auto valueNode = node.getChildByFieldName("description");
        if (!keyNode.isNull() && !valueNode.isNull()) {
            std::string key = std::string(keyNode.getSourceRange(source));
            std::string value = std::string(valueNode.getSourceRange(source));
            if (!value.empty() && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            enumVector.emplace_back(key, DataValue(value));
        }
        return DataValue(std::move(enumVector));
    }

    for (const auto &child : ts::Children{node}) {
        return handleExpression(child, source);
    }

    return DataValue("");
}


void ParsedGameData::parseValueMap(const ts::Node &node, const std::string &source, DataValue::OrderedMapType &output) {
    for (const auto &child : ts::Children{node}) {
        if (child.getType() == "map_entry") {
            ts::Node keyNode = child.getChildByFieldName("key");
            ts::Node valueNode = child.getChildByFieldName("value");

            if (!keyNode.isNull() && !valueNode.isNull()) {
                auto key = std::string(keyNode.getSourceRange(source));
                DataValue content = handleExpression(valueNode, source);
                output.emplace(key, std::move(content));
            }
        }
    }
}

DataValue::OrderedMapType ParsedGameData::handleSetup(const ts::Node &node, const std::string &source) {
    DataValue::OrderedMapType setup;

    ts::Node nameNode = node.getChildByFieldName("name");
    if (!nameNode.isNull()) {
        std::string key = std::string(nameNode.getSourceRange(source));
        DataValue::OrderedMapType content;

        ts::Node kindNode = node.getChildByFieldName("kind");
        if (!kindNode.isNull()) {
            std::string kindContent = std::string(kindNode.getSourceRange(source));
            content.emplace("kind", DataValue(kindContent));
        }

        ts::Node promptNode = node.getChildByFieldName("prompt");
        if (!promptNode.isNull()) {
            std::string promptContent = std::string(promptNode.getSourceRange(source));
            promptContent = promptContent.substr(1, promptContent.length() - 2);
            content.emplace("prompt", DataValue(promptContent));
        }

        ts::Node rangeNode = node.getChildByFieldName("range");
        if (!rangeNode.isNull()) {
            auto rangeValue = handleExpression(rangeNode, source);
            content.emplace("range", std::move(rangeValue));
        }

        ts::Node choiceNode = node.getChildByFieldName("choices");
        if (!choiceNode.isNull()) {
            DataValue::EnumDescriptionType enumVector;
            for (const auto &child : ts::Children{choiceNode}) {
                auto key = std::string(child.getSourceRange(source));
                auto value = handleExpression(child, source);
                enumVector.emplace_back(key, std::move(value));
            }
            content.emplace("choices", DataValue(std::move(enumVector)));
        }

        ts::Node defaultNode = node.getChildByFieldName("default");
        if (!defaultNode.isNull()) {
            auto defaultValue = handleExpression(defaultNode, source);
            content.emplace("default", std::move(defaultValue));
        }

        setup.emplace(key, DataValue(std::move(content)));
    }

    return setup;
}


void ParsedGameData::parseConstantsSection(const ts::Node &node, const string &source) {
    parseValueMap(node.getChildByFieldName("map"), source, constants);
}

void ParsedGameData::parseVariablesSection(const ts::Node &node, const string &source) {
    parseValueMap(node.getChildByFieldName("map"), source, variables);
}

void ParsedGameData::parsePerPlayerSection(const ts::Node &node, const string &source) {
    parseValueMap(node.getChildByFieldName("map"), source, perPlayer);
}

void ParsedGameData::parsePerAudienceSection(const ts::Node &node, const string &source) {
    parseValueMap(node.getChildByFieldName("map"), source, perAudience);
}

/*
    configuration {
        name: "Rock, Paper, Scissors"
        player range: (2, 4)
        audience: false
        setup: {
            rounds {
            kind: integer
            prompt: "The number of rounds to play"
            range: (1, 20)
            }
        }
    }
*/

void ParsedGameData::parseConfigurationSection(const ts::Node &node, const string &source) {
    ts::Node nameNode = node.getChildByFieldName("name");
    if (!nameNode.isNull()) {
        configuration.name = string(nameNode.getChild(1).getSourceRange(source));
    }

    ts::Node playerRangeNode = node.getChildByFieldName("player_range");
    if (!playerRangeNode.isNull()) {
        string rangeString = string(playerRangeNode.getSourceRange(source));
        std::regex rangeRegex(R"(\((\d+),\s*(\d+)\))");
        std::smatch match;

        if (std::regex_search(rangeString, match, rangeRegex) && match.size() == 3) {
            int min = std::stoi(match[1].str());
            int max = std::stoi(match[2].str());
            configuration.range = {min, max};
        } else {
            std::cerr << "Error: Invalid range format in player_range" << std::endl;
        }
    }

    ts::Node audienceNode = node.getChildByFieldName("has_audience");
    if (!audienceNode.isNull()) {
        auto curr = string(audienceNode.getSourceRange(source));
        configuration.audience = curr == "true" ? true : false;
    }

    // parse setup section
    size_t start = 10;
    size_t size = node.getNumChildren() - start - 1;
    for (size_t i = 0; i < size; ++i) {
        ts::Node curr = node.getChild(start++);
        auto setupEntry = handleSetup(curr, source);
        configuration.setup.emplace_back(std::move(setupEntry));
    }
}

void ParsedGameData::parseConfig(const string &fileContent) {
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};
    ts::Tree tree = parser.parseString(fileContent);
    ts::Node root = tree.getRootNode();

    // for debugging
    // printTree(root, fileContent);

    for (size_t i = 0; i < root.getNumNamedChildren(); ++i) {
        ts::Node curr = root.getNamedChild(i);
        string sectionType = string(curr.getType());

        if (sectionType == "configuration") {
            parseConfigurationSection(curr, fileContent);
        } else if (sectionType == "constants") {
            parseConstantsSection(curr, fileContent);
        } else if (sectionType == "variables") {
            parseVariablesSection(curr, fileContent);
        } else if (sectionType == "per_player") {
            parsePerPlayerSection(curr, fileContent);
        } else if (sectionType == "per_audience") {
            parsePerAudienceSection(curr, fileContent);
        } else if (sectionType == "rules") {
            // auto rule = std::make_unique<Rule>();
            // parseRuleSection(curr, fileContent, rule.get());
            // rules.push_back(std::move(rule));
            parseRuleSection(curr, fileContent);
        }
    }
}

// This will be removed eventually 
string ParsedGameData::ruleTypeToString(RuleT::Type type) {
    switch (type) {
    case RuleT::Type::For:
        return "For";
    case RuleT::Type::Loop:
        return "Loop";
    case RuleT::Type::ParallelFor:
        return "ParallelFor";
    case RuleT::Type::InParallel:
        return "InParallel";
    case RuleT::Type::Match:
        return "Match";
    case RuleT::Type::Extend:
        return "Extend";
    case RuleT::Type::Reverse:
        return "Reverse";
    case RuleT::Type::Shuffle:
        return "Shuffle";
    case RuleT::Type::Sort:
        return "Sort";
    case RuleT::Type::Deal:
        return "Deal";
    case RuleT::Type::Discard:
        return "Discard";
    case RuleT::Type::Timer:
        return "Timer";
    case RuleT::Type::InputChoice:
        return "InputChoice";
    case RuleT::Type::InputText:
        return "InputText";
    case RuleT::Type::InputVote:
        return "InputVote";
    case RuleT::Type::InputRange:
        return "InputRange";
    case RuleT::Type::Message:
        return "Message";
    case RuleT::Type::Scores:
        return "Scores";
    case RuleT::Type::Assignment:
        return "Assignment";
    case RuleT::Type::Body:
        return "Body";
    default:
        return "Unknown";
    }
}

void ParsedGameData::DFS(const ts::Node& node, const std::string& source, std::string& str){
    // Check if node is a leaf or an identifier
    if (!node.getNumNamedChildren() || node.getType() == "identifier") {
        auto content = std::string(node.getSourceRange(source));
        // Only proceed if `content` is not in the `toSkip` list
        if (find(begin(GameConstantsType::toSkip), end(GameConstantsType::toSkip), content) == end(GameConstantsType::toSkip)) {
            str = content + str;
            return;
        }
    }
    // Skip "builtin" nodes
    if (node.getType() == "builtin") {
        return;
    }

    for (const auto &child : ts::Children{node}) {
        DFS(child, source, str);
    }
}

void ParsedGameData::handleForRule(const ts::Node& node, const std::string& source) {
    ts::Node elementNode = node.getChildByFieldName("element"); // round or weapon
    ts::Node listNode = node.getChildByFieldName("list");       // configuration.rounds or weapons
    ts::Node bodyNode = node.getChildByFieldName("body");

    auto iteratorName = std::string(elementNode.getSourceRange(source));

    std::string listContent;
    if (!listNode.isNull()) {
        DFS(listNode, source, listContent);
    }
    std::unique_ptr<Rule> conditions = listContent.empty() ? nullptr : std::make_unique<StringRule>(listContent);

    std::vector<std::unique_ptr<Rule>> content;
    if (!bodyNode.isNull()) {
        for (const auto& child : ts::Children{bodyNode}) {
            auto subRule = parseRuleSection(child, source);
            if (subRule) {
                content.emplace_back(std::move(subRule));
            }
        }
    }

    std::unique_ptr<ForRule> forRule = std::make_unique<ForRule>(
    iteratorName, std::move(conditions), std::move(content));
    rules.emplace_back(std::move(forRule));
}


void ParsedGameData::handleMessageSection(const ts::Node& node, const std::string& source) {
    auto playersKeyword = node.getChildByFieldName("players").getSourceRange(source); // Keyword indicating players
    auto content = node.getChildByFieldName("content").getSourceRange(source);        // Message content

    // currently we assume all will be there all the time
    // TODO: we need to create more rule types to deal with mutiple keyword
    auto allPlayersRule = std::make_unique<AllPlayersRule>();  
    auto stringRule = std::make_unique<StringRule>(content);

    auto messageRule = std::make_unique<MessageRule>(server, *allPlayersRule, *stringRule);
    rules.emplace_back(std::move(messageRule));
}

// TODO: will have to modify logic for future rule object
void ParsedGameData::traverseHelper(const ts::Node& node, const string& source, Rule& rule){
    // if(node.getType() == "match_entry"){
    //     ts::Node guard = node.getChildByFieldName("guard");
    //     DFS(guard, source, rule);
    //     ts::Node body = node.getChildByFieldName("body");
    //     parseRuleSection(body, source, rule);
    // }

    // for (const auto &child : ts::Children{node}) {
    //     traverseHelper(child, source, rule);
    // }
}

// TODO: will have to modify logic for future rule object
void ParsedGameData::handleMatchRule(const ts::Node &node, const string &source, Rule &outerRule) {
    // ts::Node targetNode = node.getChildByFieldName("target"); // True
    // for (const auto &child : ts::Children{targetNode}) {
    //     DFS(child, source, outerRule);
    // }

    // for (size_t i = 3; i < node.getNumChildren() - 1; ++i) {
    //     auto curr = node.getChild(i);
    //     Rule subRule;
    //     // TODO: need to figure out how to call it
    //     // auto subRule = std::make_unique<MatchRule>();
    //     traverseHelper(curr, source, subRule);
    //     outerRule.subRules.emplace_back(std::move(subRule));
    // }
}

// TODO: need to check node type or how to use in txt file.
void ParsedGameData::handleWhileSection(const ts::Node& node, const std::string& source, Rule& outerRule){
    // ts::Node condition = node.getChildByFieldName("condition");
    // ts::Node loopBody = node.getChildByFieldName("body");
    // if (!condition.isNull()) {
    //     DFS(condition, source, outerRule);
    // }
    // if (!loopBody.isNull()) {
    //     for (const auto &child : ts::Children{loopBody}) {
    //         Rule subRule;
    //         DFS(loopBody, source, subRule);
    //         if(!subRule.parameters.empty()){
    //             outerRule.subRules.emplace_back(std::move(subRule));
    //         }
    //     }
    // }
}

std::unique_ptr<Rule> ParsedGameData::parseRuleSection(const ts::Node& node, const std::string& source){
    for (const auto& child : ts::Children{node}) {
        std::string_view ruleType = child.getType();

        if (ruleType == "for") {
            handleForRule(child, source);
        } else if(ruleType == "parallel_for"){
            handleForRule(child, source);
        }
        else if (ruleType == "match") {
            // outerRule.type = getRuleType(std::string(ruleType));
            // TODO: figure out how to call constructor correctly
            // outerRule = std::make_unique<MatchRule>();
            // handleMatchRule(child, source, outerRule);
        }
        else if (ruleType == "message") {
            handleMessageSection(child, source);
        }
        else if(ruleType == "loop"){
            // outerRule.type = getRuleType(std::string(ruleType));
            // outerRule = std::make_unique<LoopRule>();
            // TODO: figure out how to call constructor correctly
            // handleWhileSection(child, source, outerRule);
            // outerRule.subRules.emplace_back(whileRule);
        }
        else {
            // Recursively handle other types of rules
            parseRuleSection(child, source);
        }
    }
    return nullptr;
}

// This will be removed eventually
RuleT::Type ParsedGameData::getRuleType(const string &type) {
    string sanitizedType = type;
    sanitizedType.erase(std::remove_if(sanitizedType.begin(), sanitizedType.end(), ::isspace), sanitizedType.end());
    std::transform(sanitizedType.begin(), sanitizedType.end(), sanitizedType.begin(), ::tolower);

    static const map<string, RuleT::Type> typeMap = {{"for", RuleT::Type::For},
                                                    {"loop", RuleT::Type::Loop},
                                                    {"parallel_for", RuleT::Type::ParallelFor},
                                                    {"in_parallel", RuleT::Type::InParallel},
                                                    {"match", RuleT::Type::Match},
                                                    {"extend", RuleT::Type::Extend},
                                                    {"reverse", RuleT::Type::Reverse},
                                                    {"shuffle", RuleT::Type::Shuffle},
                                                    {"sort", RuleT::Type::Sort},
                                                    {"deal", RuleT::Type::Deal},
                                                    {"discard", RuleT::Type::Discard},
                                                    {"timer", RuleT::Type::Timer},
                                                    {"input_choice", RuleT::Type::InputChoice},
                                                    {"input_text", RuleT::Type::InputText},
                                                    {"input_vote", RuleT::Type::InputVote},
                                                    {"input_range", RuleT::Type::InputRange},
                                                    {"message", RuleT::Type::Message},
                                                    {"scores", RuleT::Type::Scores},
                                                    {"assignment", RuleT::Type::Assignment},
                                                    {"body", RuleT::Type::Body}};

    auto it = typeMap.find(sanitizedType);
    if (it != typeMap.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Unknown rule type: " + sanitizedType);
    }
}

void ParsedGameData::printTree(const ts::Node &node, const string &source, int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";
    std::cout << node.getType() << " -> " << node.getSourceRange(source) << std::endl;

    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        printTree(node.getNamedChild(i), source, indent + 1);
    }
}

void ParsedGameData::printDataValue(const DataValue::OrderedMapType &value, int indent) {
    std::string indentStr(indent, ' ');

    for (const auto &[key, dataValue] : value) {
        std::cout << indentStr << "\"" << key << "\": ";
        printSingleDataValue(dataValue, indent + 2); // Print each DataValue with increased indentation
    }
}

// Helper function to print a single DataValue
void ParsedGameData::printSingleDataValue(const DataValue &value, int indent) {
    std::string indentStr(indent, ' ');

    // Check the type of the DataValue and print accordingly
    if (std::holds_alternative<std::string>(value.getValue())) {
        std::cout << indentStr << "\"" << std::get<std::string>(value.getValue()) << "\"\n";
    } else if (std::holds_alternative<int>(value.getValue())) {
        std::cout << indentStr << std::get<int>(value.getValue()) << "\n";
    } else if (std::holds_alternative<bool>(value.getValue())) {
        std::cout << indentStr << (std::get<bool>(value.getValue()) ? "true" : "false") << "\n";
    } else if (std::holds_alternative<std::vector<DataValue>>(value.getValue())) {
        std::cout << indentStr << "[\n";
        for (const auto &item : std::get<std::vector<DataValue>>(value.getValue())) {
            printSingleDataValue(item, indent + 2); // Recursive call with increased indentation
        }
        std::cout << indentStr << "]\n";
    } else if (std::holds_alternative<DataValue::OrderedMapType>(value.getValue())) {
        std::cout << indentStr << "{\n";
        for (const auto &[key, subValue] : std::get<DataValue::OrderedMapType>(value.getValue())) {
            std::cout << indentStr << "  \"" << key << "\": ";
            printSingleDataValue(subValue, indent + 2); // Recursive call for nested map values
        }
        std::cout << indentStr << "}\n";
    } else {
        std::cout << indentStr << "Unknown Type\n"; // Fallback for an unknown type
    }
}

void ParsedGameData::printKeyValuePair() {
    std::cout << "\nConfiguration Section:" << std::endl;
    std::cout << "name: " << configuration.name << std::endl;
    std::cout << "player range: (" << configuration.range.first << ", " << configuration.range.second << ")"
              << std::endl;
    std::cout << "audience: " << (configuration.audience == true ? "true" : "false") << std::endl;
}