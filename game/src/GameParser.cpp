// author: kwa132, Mtt8

#include "GameParser.h"

extern "C" { TSLanguage *tree_sitter_socialgaming(); }

ParsedGameData::ParsedGameData(const std::string& config){
    std::string fileContent = readFileContent(config);
    if (!fileContent.empty()) {
        parseConfig(fileContent);
    }
}

std::string ParsedGameData::readFileContent(const std::string& filePath){
    std::ifstream inputFile(filePath);
    if (!inputFile) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

std::string ParsedGameData::getGameName() const {
    return configuration.name;
}

std::pair<int, int> ParsedGameData::getPlayerRange() const {
    return configuration.range;
}

bool ParsedGameData::hasAudience() const {
    return configuration.audience;
}

Configuration ParsedGameData::getConfiguration(){
    return configuration;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> ParsedGameData::getConstants(){
    return constants;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> ParsedGameData::getVariables(){
    return variables;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> ParsedGameData::getPerPlayer(){
    return perPlayer;
}

const std::vector<std::map<std::string, std::vector<std::map<std::string, std::string>>>>& ParsedGameData::getSetup() const{
    return configuration.setup;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> ParsedGameData::getPerAudience() {
    return perAudience;
}

std::vector<Rule> ParsedGameData::getRules(){
    return rules;
}

void ParsedGameData::extractStringValue(const ts::Node& node, const std::string& source, 
    std::pair<std::string, std::string>& str1, std::pair<std::string, std::string>& str2, std::string keyID, 
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>& output) {

    if (!node.getNumNamedChildren()) {
        auto byteRange = node.getByteRange();
        auto curr = source.substr(byteRange.start, byteRange.end - byteRange.start);
        if (find(begin(GameConstantsType::toSkip), end(GameConstantsType::toSkip), curr) == end(GameConstantsType::toSkip)) {
            curr.erase(std::remove_if(curr.begin(), curr.end(), [](char c) {
                return c == ':' || isspace(c);
            }), curr.end());
            str1.first.empty() ? str1.first = curr : (str1.second.empty() ? str1.second = curr :
            (str2.first.empty() ? str2.first = curr : str2.second = curr));
        }
        return;
    }

    for (size_t i = 0; i < node.getNumChildren(); ++i) {
        ts::Node child = node.getChild(i);
        auto type = std::string(child.getType());
        if (find(begin(GameConstantsType::toSkip), end(GameConstantsType::toSkip), type) != end(GameConstantsType::toSkip)) {
            continue;
        }
        extractStringValue(child, source, str1, str2, keyID, output);
    }

    if (!str1.first.empty() && str1.second.empty() && str2.first.empty() && str2.second.empty()) {
        output[keyID].push_back({{str1.first, ""}, {"", ""}});
        str1 = {};
        str2 = {};
    } else if (!str1.first.empty() && !str1.second.empty() && !str2.first.empty()) {
        output[keyID].push_back({str1, str2});
        str1 = {};
        str2 = {};
    }
}

template <typename T>
void ParsedGameData::parseValueMap(const ts::Node& node, const std::string& source, T& outputMap) {
    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        ts::Node entryNode = node.getNamedChild(i);
        if (entryNode.getType() == "map_entry") {
            ts::Node keyNode = entryNode.getChildByFieldName("key");
            ts::Node valueNode = entryNode.getChildByFieldName("value");
            if (!keyNode.isNull() && !valueNode.isNull()) {
                auto byteRange = keyNode.getByteRange();
                std::string key = source.substr(byteRange.start, byteRange.end - byteRange.start);
                std::pair<std::string, std::string> str1;
                std::pair<std::string, std::string> str2;
                extractStringValue(valueNode, source, str1, str2, key, outputMap);   
            }
        }
    }
}

void ParsedGameData::setupHelper(const ts::Node& node, const std::string& source, std::string& str1, std::string& str2, const std::string& keyID
    , std::map<std::string, std::vector<std::map<std::string, std::string>>> &inputSetup) {
    auto type = std::string(node.getType());
    if (node.getType() == "number_range") {
        std::string rangeStr;
        for (const auto& child : ts::Children{node}) {
            if (child.getType() == "number") {
                std::string res = std::string(child.getSourceRange(source));
                rangeStr.empty() ? rangeStr = res : rangeStr += ", " + res;
            }
        }

        str2 = rangeStr;
        std::map<std::string, std::string> tempMap;
        tempMap[str1] = str2;
        inputSetup[keyID].push_back(tempMap);

        str1.clear();
        str2.clear();
        return;  
    }

    if (node.getNumChildren() == 0 && (find(begin(GameConstantsType::toSkip), end(GameConstantsType::toSkip), type) == end(GameConstantsType::toSkip))) {
        std::string res = std::string(node.getSourceRange(source));
        if (!res.empty()) {
            size_t pos = res.find(":");
            if (pos != std::string::npos && pos == res.size() - 1) {
                res.erase(pos);  
            }
        }

    str1.empty() ? str1 = res : str2 = res;
}

    for (const auto& child : ts::Children{node}) {
        setupHelper(child, source, str1, str2, keyID, inputSetup);
    }

    if (!str1.empty() && !str2.empty()) {
        std::map<std::string, std::string> tempMap;
        tempMap[str1] = str2;
        inputSetup[keyID].push_back(tempMap);

        str1.clear();
        str2.clear();
    }
}

void ParsedGameData::parsePerPlayerSection(const ts::Node& node, const std::string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, perPlayer);
}

void ParsedGameData::parsePerAudienceSection(const ts::Node& node, const std::string& source) {
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

void ParsedGameData::parseConfigurationSection(const ts::Node& node, const std::string& source) {
    ts::Node nameNode = node.getChildByFieldName("name");
    if (!nameNode.isNull()) {
        configuration.name = std::string(nameNode.getChild(1).getSourceRange(source));
    }

    ts::Node playerRangeNode = node.getChildByFieldName("player_range");
    if (!playerRangeNode.isNull()) {
        std::string rangeString = std::string(playerRangeNode.getSourceRange(source));
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
        auto curr = std::string(audienceNode.getSourceRange(source));
        configuration.audience = curr == "true" ? true : false;
    }

    ts::Node setupNode = node.getChild(10); // set_rule
    auto size = setupNode.getNumChildren();
    ts::Node nestedNode = setupNode.getChildByFieldName("name");
    auto key = std::string(nestedNode.getSourceRange(source));

    std::string str1 = "";
    std::string str2 = "";
    ts::Cursor cursor = nestedNode.getNextSibling().getCursor();
    auto curr = cursor.getCurrentNode();
    std::map<std::string, std::vector<std::map<std::string, std::string>>> subSetup;
    for(size_t i = 0; i < size - 1; ++i){
        setupHelper(curr, source, str1, str2, key, subSetup);
        if(!curr.isNull()){
            curr = curr.getNextSibling();
        }
    }
    configuration.setup.emplace_back(subSetup);
}


void ParsedGameData::parseConstantsSection(const ts::Node& node, const std::string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, constants);
}

void ParsedGameData::parseVariablesSection(const ts::Node& node, const std::string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, variables);
}

void ParsedGameData::parseConfig(const std::string& fileContent) {
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};
    ts::Tree tree = parser.parseString(fileContent);
    ts::Node root = tree.getRootNode();

    // for debugging
    // printTree(root, fileContent);

    for (size_t i = 0; i < root.getNumNamedChildren(); ++i) {
        ts::Node curr = root.getNamedChild(i);
        std::string sectionType = std::string(curr.getType());

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
            Rule rule;
            parseRuleSection(curr, fileContent, rule);
            rules.push_back(rule);
        }
    }
}

std::string ParsedGameData::ruleTypeToString(Rule::Type type){
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

void ParsedGameData::DFS(const ts::Node& node, const std::string& source, Rule& rule){
    // Check if node is a leaf or an identifier
    if (!node.getNumNamedChildren() || node.getType() == "identifier") {
        auto content = std::string(node.getSourceRange(source));
        if (find(begin(GameConstantsType::toSkip), end(GameConstantsType::toSkip), content) == end(GameConstantsType::toSkip)) {
            auto identifier = node.getSourceRange(source);
            
            auto exists = std::find_if(rule.parameters.begin(), rule.parameters.end(), [&](const auto& param) {
                return std::holds_alternative<std::string>(param) && std::get<std::string>(param) == identifier;
            }) != rule.parameters.end();
            
            if (exists) {
                return;
            }
            // type is either certain rule type or body
            if(rule.type == Rule::Type::Default){
                rule.type = Rule::Type::Body;
            }
            if (std::all_of(identifier.begin(), identifier.end(), ::isdigit)) {
                rule.parameters.emplace_back(std::stoi(std::string(identifier)));
            } else {
                rule.parameters.emplace_back(std::string(identifier));
            }
        }
    }
    if (node.getType() == "builtin") {
        return;
    }

    for (const auto& child : ts::Children{node}) {
        DFS(child, source, rule);
    }    
}

void ParsedGameData::handleForRule(const ts::Node& node, const std::string& source, Rule& outerRule){

    ts::Node elementNode = node.getChildByFieldName("element"); // round or weapon
    ts::Node listNode = node.getChildByFieldName("list");       // configuration.rounds or weapons
    ts::Node bodyNode = node.getChildByFieldName("body");

    if (!elementNode.isNull()) {
        outerRule.parameters.emplace_back(std::string(elementNode.getSourceRange(source)));
    }

    if(!listNode.isNull()){
        DFS(listNode, source, outerRule);
    }
    
    if (!bodyNode.isNull()) {
        for (const auto& child : ts::Children{bodyNode}) {
            Rule subRule;
            parseRuleSection(child, source, subRule);
            if(!subRule.parameters.empty()){
                outerRule.subRules.emplace_back(subRule);
            }
        }
    } 
}

void ParsedGameData::handleMessageSection(const ts::Node& node, const std::string& source, Rule& outerRule){
    if(!node.getNumNamedChildren()){
        auto currContent = std::string(node.getSourceRange(source));
        if(currContent != "\"" && currContent != ";"){
            outerRule.parameters.emplace_back(currContent);
        }
        return;
    }
    for(const auto& child : ts::Children{node}){
        handleMessageSection(child, source, outerRule);
    }    
}

void ParsedGameData::traverseHelper(const ts::Node& node, const std::string& source, Rule& rule){
    if(node.getType() == "match_entry"){
        ts::Node guard = node.getChildByFieldName("guard");
        DFS(guard, source, rule);
        ts::Node body = node.getChildByFieldName("body");
        parseRuleSection(body, source, rule);
    }

    for(const auto& child : ts::Children{node}){
        traverseHelper(child, source, rule);
    }    
}

void ParsedGameData::handleMatchRule(const ts::Node& node, const std::string& source, Rule& outerRule){
    ts::Node targetNode = node.getChildByFieldName("target");  // True
    for(const auto& child : ts::Children{targetNode}){
        DFS(child, source, outerRule);
    }

    for(size_t i = 3; i < node.getNumChildren() - 1; ++i){
        auto curr = node.getChild(i);
        Rule subRule;
        traverseHelper(curr, source, subRule);
        outerRule.subRules.emplace_back(subRule);
    }  
}

// TODO: need to check node type or how to use in txt file.
void ParsedGameData::handleWhileSection(const ts::Node& node, const std::string& source, Rule& outerRule){
    ts::Node condition = node.getChildByFieldName("condition");
    ts::Node loopBody = node.getChildByFieldName("body");
    if(!condition.isNull()){
        DFS(condition, source, outerRule);
    }
    if(!loopBody.isNull()){
        for(const auto& child : ts::Children{loopBody}){
            Rule subRule;
            DFS(loopBody, source, subRule);
            if(!subRule.parameters.empty()){
                outerRule.subRules.emplace_back(subRule);
            }
        }
    }     
}

void ParsedGameData::parseRuleSection(const ts::Node& node, const std::string& source, Rule& outerRule){
    for (const auto& child : ts::Children{node}) {
        std::string_view ruleType = child.getType();

        if (ruleType == "for") {
            outerRule.type = getRuleType(std::string(ruleType));
            handleForRule(child, source, outerRule);
        } else if(ruleType == "parallel_for"){
            outerRule.type = getRuleType(std::string(ruleType));
            handleForRule(child, source, outerRule);
        }
        else if (ruleType == "match") {
            outerRule.type = getRuleType(std::string(ruleType));
            handleMatchRule(child, source, outerRule);
        }
        else if (ruleType == "message") {
            outerRule.type = getRuleType(std::string(ruleType));
            handleMessageSection(child, source, outerRule);
        }
        else if(ruleType == "loop"){
            outerRule.type = getRuleType(std::string(ruleType));
            handleWhileSection(child, source, outerRule);
        }
        else {
            parseRuleSection(child, source, outerRule);
        }
    }    
}

Rule::Type ParsedGameData::getRuleType(const std::string& type) {
    std::string sanitizedType = type;
    sanitizedType.erase(std::remove_if(sanitizedType.begin(), sanitizedType.end(), ::isspace), sanitizedType.end());
    std::transform(sanitizedType.begin(), sanitizedType.end(), sanitizedType.begin(), ::tolower);

    static const std::map<std::string, Rule::Type> typeMap = {
        {"for", Rule::Type::For},
        {"loop", Rule::Type::Loop},
        {"parallel_for", Rule::Type::ParallelFor},
        {"in_parallel", Rule::Type::InParallel},
        {"match", Rule::Type::Match},
        {"extend", Rule::Type::Extend},
        {"reverse", Rule::Type::Reverse},
        {"shuffle", Rule::Type::Shuffle},
        {"sort", Rule::Type::Sort},
        {"deal", Rule::Type::Deal},
        {"discard", Rule::Type::Discard},
        {"timer", Rule::Type::Timer},
        {"input_choice", Rule::Type::InputChoice},
        {"input_text", Rule::Type::InputText},
        {"input_vote", Rule::Type::InputVote},
        {"input_range", Rule::Type::InputRange},
        {"message", Rule::Type::Message},
        {"scores", Rule::Type::Scores},
        {"assignment", Rule::Type::Assignment},
        {"body", Rule::Type::Body}
    };

    auto it = typeMap.find(sanitizedType);
    if (it != typeMap.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Unknown rule type: " + sanitizedType);
    }
}

void ParsedGameData::printTree(const ts::Node& node, const std::string& source, int indent){
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node.getType() << " -> " << node.getSourceRange(source) << std::endl;

    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        printTree(node.getNamedChild(i), source, indent + 1);
    }
}

void ParsedGameData::printKeyValuePair() {
    std::cout << "\nConfiguration Section:" << std::endl;
    std::cout << "name: " << configuration.name << std::endl;
    std::cout << "player range: (" << configuration.range.first << ", " << configuration.range.second << ")" << std::endl;
    std::cout << "audience: " << (configuration.audience == true ? "true" : "false") << std::endl;


    std::cout << "\nSetup Section:" << std::endl;
    for(const auto& setup : configuration.setup){
        for(const auto& [key, value] : setup){
            std::cout << key << ":" << std::endl;
            for(const auto& ele : value){
                for(const auto& [k, v] : ele){
                    std::cout << k << ": " << v << std::endl;
                }
            }
        }
    }

    for (const auto& [key, entries] : variables) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            std::cout << std::endl;
        }   
    }

    for (const auto& [key, entries] : perPlayer) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            std::cout << std::endl;
        }   
    }
    for (const auto& [key, entries] : perAudience) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            std::cout << std::endl;
        }   
    }
}