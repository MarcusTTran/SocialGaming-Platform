// author: kwa132, Mtt8

#include "GameParser.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

extern "C" {
TSLanguage *tree_sitter_socialgaming();
}

ParsedGameData::ParsedGameData(const string &config, std::shared_ptr<IServer> server, networking::Connection connection)
    : server(std::static_pointer_cast<Messenger>(server)), gameConnection(connection) {
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

const vector<std::unique_ptr<Rule>> &ParsedGameData::getRules() const { return rules; }

vector<std::unique_ptr<Rule>> ParsedGameData::moveRules() { return std::move(rules); }

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
        std::cout << "handleExpression function" << std::endl; // TODO: remove
        return DataValue(curr);
    } else if (type == "number_range") {
        size_t start = currContent.find('(');
        size_t comma = currContent.find(',');
        size_t end = currContent.find(')');
        if (start != std::string::npos && comma != std::string::npos && end != std::string::npos) {
            try {
                std::cout << "handleExpression function lower" << std::endl; // TODO: remove
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
            // promptContent = promptContent.substr(1, promptContent.length() - 2);
            promptContent = promptContent.substr(2, promptContent.length() - 3);

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
            std::cout << "parseConfiguration function" << std::endl; // TODO: remove
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
    // 10 is the magic number where the correct node to access set up node
    // based on tree-sitter layout
    size_t start = 10;
    size_t size = node.getNumChildren() - start - 2;
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

            // Parse rules
            ts::Node rulesNode = curr.getChildByFieldName("body");
            for (const auto &child : ts::Children{rulesNode}) {
                auto rule = parseRuleSection(child, fileContent);
                if (rule) {
                    rules.emplace_back(std::move(rule));
                    std::cout << "Rule added to top level rules" << std::endl;
                } else {
                    std::cerr << "Error: Rule is null" << std::endl;
                }
                std::cout << "TEST" << std::endl;
            }
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

// mainContent = {player, name}, choose your weapon!
// variables = player, name
void ParsedGameData::DFS(const ts::Node &node, const std::string &source, std::vector<std::string> &mainContent) {
    // Check if node is a leaf or an identifier
    if (!node.getNumNamedChildren() || node.getType() == "identifier") {
        auto content = std::string(node.getSourceRange(source));
        if (find(begin(GameConstantsType::toSkip), end(GameConstantsType::toSkip), content) ==
            end(GameConstantsType::toSkip)) {
            mainContent.emplace_back(content);
            return;
        }
    }

    for (const auto &child : ts::Children{node}) {
        if (child.getType() == "builtin") {
            return;
        }
        DFS(child, source, mainContent);
    }
}

std::unique_ptr<Rule> ParsedGameData::handleBuiltin(const ts::Node &node, const std::string &source,
                                                    std::unique_ptr<Rule> rule, std::vector<DataValue> list) {
    auto content = node.getSourceRange(source);

    if (content.find("upfrom") != std::string::npos) {
        std::string strVal = std::string(node.getNextSibling().getSourceRange(source));
        if (!strVal.empty() && strVal.front() == '(' && strVal.back() == ')') {
            strVal.erase(strVal.begin());
            strVal.erase(strVal.end() - 1);
        }
        int value = std::stoi(strVal);
        return std::make_unique<UpfromRule>(std::move(rule), value);
    } else if (content.find("contains") != std::string::npos) {
        std::cout << "THIS IS CONTAINS" << std::endl;
        std::string strVal = std::string(node.getNextSibling().getSourceRange(source));
        if (!strVal.empty() && strVal.front() == '(' && strVal.back() == ')') {
            strVal.erase(strVal.begin());
            strVal.erase(strVal.end() - 1);
        }
        return std::make_unique<ContainsRule>(std::move(list), DataValue{strVal});
    } else if (content.find("collect") != std::string::npos) {
        std::cout << "THIS IS COLLECT" << std::endl;
        // TODO: Create and return CollectRule
        // return std::make_unique<CollectRule>();
    } else {
        std::cout << "THIS IS SIZE" << std::endl;
        // TODO: Create and return SizeRule
        // TODO: add ListSizeRule for size checking
        return std::make_unique<ListSizeRule>(std::move(rule));
    }

    return nullptr;
}

std::unique_ptr<Rule> ParsedGameData::handleForRule(const ts::Node &node, const std::string &source) {
    ts::Node elementNode = node.getChildByFieldName("element");
    ts::Node listNode = node.getChildByFieldName("list");
    ts::Node builtInNode = listNode.getChildByFieldName("builtin");
    ts::Node bodyNode = node.getChildByFieldName("body");

    auto iteratorName = std::string(elementNode.getSourceRange(source));

    std::vector<std::string> listContent;
    if (!listNode.isNull()) {
        DFS(listNode, source, listContent);
    }

    for (const auto &str : listContent) {
        std::cout << "List content: " << str << std::endl;
    }

    std::unique_ptr<Rule> temp = std::make_unique<NameResolverRule>(listContent);
    std::unique_ptr<Rule> conditions;
    std::vector<DataValue> dataValues;
    dataValues.reserve(listContent.size());
    for (const auto &str : listContent) {
        dataValues.emplace_back(str);
    }
    if (!builtInNode.isNull()) {
        std::cout << "BuiltInNode is not null" << std::endl;
        conditions = handleBuiltin(builtInNode, source, std::move(temp), dataValues);
    } else {
        conditions = std::move(temp);
    }

    std::vector<std::unique_ptr<Rule>> content;

    if (bodyNode.isNull()) {
        std::cout << "Body is empty" << std::endl;
    }

    if (!bodyNode.isNull()) {

        for (const auto &child : ts::Children{bodyNode}) {
            std::cout << "Body is not empty" << std::endl;

            auto subRule = parseRuleSection(child, source);
            if (subRule) {
                std::cout << "Subrule is not empty" << std::endl;
                content.emplace_back(std::move(subRule));
            }
        }
    }

    std::cout << "FOR RULE CONTENTS SIZE: " << content.size() << std::endl;

    auto forRule = std::make_unique<ForRule>(iteratorName, std::move(conditions), std::move(content));
    return forRule;
}

std::string ParsedGameData::extractAndReplacePlaceholders(const std::string &contentStr,
                                                          std::vector<std::string> &variables) {
    std::regex placeholderRegex(R"(\{([^}]+)\})");
    std::ostringstream result;

    std::string::const_iterator start = contentStr.begin();
    std::string::const_iterator end = contentStr.end();
    std::sregex_iterator matches(start, end, placeholderRegex);
    std::sregex_iterator endMatches;

    auto lastPosition = start;

    for (auto it = matches; it != endMatches; ++it) {
        std::smatch match = *it;
        result << std::string(lastPosition, match.prefix().second);
        std::string placeholder = match[1].str();
        splitString(placeholder, '.', variables);

        // Replace the placeholder with {}
        result << "{}";
        lastPosition = match.suffix().first;
    }

    result << std::string(lastPosition, end);

    return result.str();
}

void ParsedGameData::splitString(const std::string &str, char delimiter, std::vector<std::string> &parts) {
    std::istringstream ss(str);
    std::string part;
    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
}

std::unique_ptr<Rule> ParsedGameData::handleMessageSection(const ts::Node &node, const std::string &source) {
    std::cout << "handleMessageSection function" << std::endl;                        // TODO: remove
    auto playersKeyword = node.getChildByFieldName("players").getSourceRange(source); // Keyword indicating players
    auto content = node.getChildByFieldName("content").getSourceRange(source);        // Message content
    std::string contentStr = std::string(content);
    // Fix formatting of quoted strings
    if (!contentStr.empty() && contentStr.front() == '"' && contentStr.back() == '"') {
        contentStr.erase(contentStr.begin());
        contentStr.erase(contentStr.end() - 1);
    }

    //
    std::unique_ptr<Rule> recipientRule = nullptr;
    std::cout << "Players keyword: " << playersKeyword << std::endl;
    if (playersKeyword == "all") {
        recipientRule = std::make_unique<AllPlayersRule>();

    } else if (playersKeyword == "player") {
        std::cout << "THE RECIPIENT IS PLAYER" << std::endl;
        std::vector<std::string> recipientList{"player"};

        recipientRule = std::make_unique<NameResolverRule>(recipientList);
    }

    // Process content and replace placeholders with {}
    std::vector<std::string> variables;
    std::string updatedContent = extractAndReplacePlaceholders(contentStr, variables);
    std::cout << "Variablies.size() " << variables.size() << std::endl;
    std::unique_ptr<Rule> nameResolver = std::make_unique<NameResolverRule>(variables);
    // auto allPlayersRule = std::make_unique<AllPlayersRule>();
    std::vector<std::unique_ptr<Rule>> nameResolvers;
    nameResolvers.emplace_back(std::move(nameResolver));
    auto stringRule = std::make_unique<StringRule>(updatedContent, std::move(nameResolvers));
    auto messageRule = std::make_unique<MessageRule>(server, std::move(recipientRule), std::move(stringRule));
    return messageRule;
}

void ParsedGameData::traverseHelper(const ts::Node &node, const string &source,
                                    std::vector<std::unique_ptr<Rule>> &checkCondition,
                                    std::vector<std::unique_ptr<Rule>> &scopedRule) {
    if (node.getType() == "match_entry") {
        ts::Node guard = node.getChildByFieldName("guard");
        vector<std::string> condition;
        DFS(guard, source, condition);
        if (condition.size() == 1 && (condition.back() == "true" || condition.back() == "false")) {
            bool boolean = condition.back() == "true" ? true : false;
            std::unique_ptr<Rule> conditionPtr = std::make_unique<BooleanRule>(boolean);
            checkCondition.emplace_back(std::move(conditionPtr));
        } else {
            // TODO: adding more condition rules
        }
        ts::Node body = node.getChildByFieldName("body");
        std::unique_ptr<Rule> bodyPtr = parseRuleSection(body, source);
        scopedRule.emplace_back(std::move(bodyPtr));
    }

    for (const auto &child : ts::Children{node}) {
        traverseHelper(child, source, checkCondition, scopedRule);
    }
}

std::unique_ptr<Rule> ParsedGameData::handleMatchRule(const ts::Node &node, const string &source) {
    ts::Node targetNode = node.getChildByFieldName("target"); // True
    std::vector<std::string> targetConetnt;
    for (const auto &child : ts::Children{targetNode}) {
        DFS(child, source, targetConetnt);
    }

    auto hasBuiltin = targetNode.getChild(0);
    while (hasBuiltin.getType() != "expression") {
        hasBuiltin = hasBuiltin.getNextSibling();
    }
    ts::Node builtInNode = hasBuiltin.getChildByFieldName("builtin");
    std::unique_ptr<Rule> temp = std::make_unique<NameResolverRule>(targetConetnt);
    std::unique_ptr<Rule> conditions;

    std::vector<DataValue> dataValues;
    dataValues.reserve(targetConetnt.size());
    for (const auto &str : targetConetnt) {
        dataValues.emplace_back(str);
    }

    // handle builtin like contains
    // this one will be called as condition_maker for MatchRule
    if (!builtInNode.isNull()) {
        conditions = handleBuiltin(builtInNode, source, std::move(temp), dataValues);
    } else {
        conditions = std::move(temp);
    }
    std::vector<std::unique_ptr<Rule>> checkCondition;
    std::vector<std::unique_ptr<Rule>> scopedRule;
    for (size_t i = 3; i < node.getNumChildren() - 1; ++i) {
        auto curr = node.getChild(i);
        traverseHelper(curr, source, checkCondition, scopedRule);
    }
    std::unique_ptr<Rule> matchRule =
        std::make_unique<MatchRule>(std::move(conditions), std::move(checkCondition), std::move(scopedRule));
    return matchRule;
}

// TODO: need to check node type or how to use in txt file.
void ParsedGameData::handleWhileSection(const ts::Node &node, const std::string &source, Rule &outerRule) {
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

std::unique_ptr<Rule> ParsedGameData::handelInputChoice(const ts::Node &node, const std::string &source) {
    std::cout << "THIS IS INPUT CHOICE" << std::endl;
    std::string_view player = node.getChildByFieldName("player").getSourceRange(source);
    std::string_view prompt = node.getChildByFieldName("prompt").getSourceRange(source);
    std::string_view choices = node.getChildByFieldName("choices").getSourceRange(source);
    std::string_view target = node.getChildByFieldName("target").getSourceRange(source);
    std::string_view timeout = node.getChildByFieldName("timeout").getSourceRange(source);
    std::string promptStr = std::string(prompt);

    // remove quote
    if (!promptStr.empty() && promptStr.front() == '"' && promptStr.back() == '"') {
        promptStr = promptStr.substr(1, promptStr.length() - 2);
    }

    // print to console
    std::cout << "Prompt: " << promptStr << std::endl;
    std::cout << "Choices: " << choices << std::endl;
    std::cout << "Target: " << target << std::endl;
    std::cout << "Timeout: " << timeout << std::endl;

    // dealing with {}
    std::vector<std::string> promptVariables;
    promptStr = extractAndReplacePlaceholders(promptStr, promptVariables);

    std::unique_ptr<Rule> nameResolver = std::make_unique<NameResolverRule>(promptVariables);
    std::vector<std::unique_ptr<Rule>> nameResolvers;
    nameResolvers.emplace_back(std::move(nameResolver));

    std::vector<std::string> recipientList{"player"};
    auto playerRule = std::make_unique<NameResolverRule>(recipientList);

    // TODO: as messageRule, need stringRule to accept one more vec or nameResolver
    // auto stringRule = std::make_unique<StringRule>(promptStr, std::move(nameResolvers));
    // auto messageRule = std::make_unique<MessageRule>(server, std::make_unique<AllPlayersRule>(),
    // std::move(stringRule)); return messageRule;
    auto simpleInputRule = std::make_unique<SimpleInputRule>(server, target, promptStr);
    return simpleInputRule;
}

// std::unique_ptr<Rule> rule     {std::move(handleNameResolver())};

// std::unique_ptr<Rule> handleNameResolverRule(const string &source, Rule &outerRule) {
//     // std::vector<std::string> search_keys;
//     // size_t dotPosition = source.find('.');
//     // size_t start_pos = 0;

//     // while (dotPosition != std::string::npos) {
//     //     search_keys.push_back(source.substr(start_pos, dotPosition - start_pos));
//     //     start_pos = dotPosition + 1;
//     //     dotPosition = source.find('.', start_pos);
//     // }
//     // // Add last innermost key
//     // search_keys.push_back(source.substr(start_pos));

//     std::unique_ptr<Rule> nameResolverRule = std::make_unique<NameResolverRule>(search_keys);
//     return nameResolverRule;
// }

std::unique_ptr<Rule> ParsedGameData::handleDiscard(const ts::Node &node, const std::string &source) {
    // discard winners.size() from winners
    ts::Node countNode = node.getChildByFieldName("count");
    ts::Node sourceNode = node.getChildByFieldName("source");
    ts::Node builtinNode = countNode.getChildByFieldName("builtin");

    // discardConditionContent = winners, spliting from .size() which will be handled in builtin function handler
    // discardTarget = winners
    std::vector<std::string> discardConditionContent;
    std::vector<std::string> discardTargetContent;
    DFS(countNode, source, discardConditionContent);
    DFS(countNode, source, discardTargetContent);

    std::unique_ptr<Rule> temp = std::make_unique<NameResolverRule>(discardConditionContent);
    std::unique_ptr<Rule> conditions;
    std::vector<DataValue> dataValues;
    dataValues.reserve(discardConditionContent.size());
    for (const auto &str : discardConditionContent) {
        dataValues.emplace_back(str);
    }

    if (!builtinNode.isNull()) {
        conditions = handleBuiltin(builtinNode, source, std::move(temp), dataValues);
    } else {
        conditions = std::move(temp);
    }

    std::unique_ptr<Rule> discardTarget = std::make_unique<NameResolverRule>(discardTargetContent);
    std::unique_ptr<Rule> discardRule = std::make_unique<DiscardRule>(std::move(conditions), std::move(discardTarget));
    return discardRule;
}

std::unique_ptr<Rule> ParsedGameData::handleScore(const ts::Node &node, const std::string &source) {
    ts::Node scoreNode = node.getChildByFieldName("keys");
    std::vector<std::string> keysContent;
    DFS(scoreNode, source, keysContent);
    std::unique_ptr<Rule> scoreRule = std::make_unique<ScoresRule>(keysContent, gameConnection, server);
    return scoreRule;
}

// extend winners with players.elements.collect(player, player.weapon = weapon.beats);
std::unique_ptr<Rule> ParsedGameData::handleExtend(const ts::Node &node, const std::string &source) {
    ts::Node targetNode = node.getChildByFieldName("target");
    ts::Node valueNode = node.getChildByFieldName("value");
    ts::Node builtinNode = valueNode.getChildByFieldName("builtin");

    std::vector<std::string> targetContent;
    std::vector<std::string> valueContent;
    DFS(targetNode, source, targetContent);
    DFS(valueNode, source, valueContent);

    // Check for "elements" keyword and split elements if present
    bool hasElements = std::find(valueContent.begin(), valueContent.end(), "elements") != valueContent.end();
    std::vector<std::string> splitElement;

    if (hasElements) {
        std::copy_if(valueContent.begin(), valueContent.end(), std::back_inserter(splitElement),
                     [](const std::string &value) { return value != "elements"; });
    }

    std::unique_ptr<Rule> valuePtr;
    if (hasElements) {
        if (!builtinNode.isNull()) {
            valuePtr = std::make_unique<ElementsRule>(
                std::make_unique<NameResolverRule>(splitElement),
                std::string(builtinNode.getSourceRange(source)));
        } else {
            valuePtr = std::make_unique<NameResolverRule>(valueContent);
        }
    } else {
        valuePtr = std::make_unique<NameResolverRule>(valueContent);
    }

    std::unique_ptr<Rule> condition = builtinNode.isNull()
                                          ? std::move(valuePtr)
                                          : handleBuiltin(builtinNode, source, std::move(valuePtr), {});


    std::unique_ptr<Rule> target = std::make_unique<NameResolverRule>(targetContent);

    return std::make_unique<ExtendRule>(std::move(target), std::move(condition));
}


std::unique_ptr<Rule> ParsedGameData::parseRuleSection(const ts::Node &node, const std::string &source) {
    std::unique_ptr<Rule> parsedRule = nullptr;

    for (const auto &child : ts::Children{node}) {
        std::string_view ruleType = child.getType();

        if (ruleType == "for") {
            parsedRule = handleForRule(child, source);
        } else if (ruleType == "parallel_for") {
            parsedRule = handleForRule(child, source);
        } else if (ruleType == "match") {
            parsedRule = handleMatchRule(child, source);
        } else if (ruleType == "message") {
            parsedRule = handleMessageSection(child, source);
        } else if (ruleType == "loop") {
            // outerRule.type = getRuleType(std::string(ruleType));
            // outerRule = std::make_unique<LoopRule>();
            // TODO: figure out how to call constructor correctly
            // handleWhileSection(child, source, outerRule);
            // outerRule.subRules.emplace_back(whileRule);
        } else if (ruleType == "input_choice") {
            parsedRule = handelInputChoice(child, source);
        } else if (ruleType == "discard") {
            parsedRule = handleDiscard(child, source);
        } else if (ruleType == "scores") {
            parsedRule = handleScore(child, source);
        } else if (ruleType == "extend"){
            parsedRule = handleExtend(child, source);
        }
        else {
            // Recursively handle other types of rules
            parsedRule = parseRuleSection(child, source);
        }

        if (parsedRule) {
            return parsedRule;
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
