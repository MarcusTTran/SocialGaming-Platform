// author: kwa132, Mtt8

#include "GameParser.h"

extern "C" { TSLanguage *tree_sitter_socialgaming(); }

GameConfig::GameConfig(const std::string& config){
    std::string fileContent = readFileContent(config);
    if (!fileContent.empty()) {
        parseConfig(fileContent);
    }
}

std::string GameConfig::readFileContent(const std::string& filePath){
    std::ifstream inputFile(filePath);
    if (!inputFile) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

std::string GameConfig::getGameName() const {
    return configuration.find("name") != configuration.end() ? configuration.at("name") : "";
}

std::pair<int, int> GameConfig::getPlayerRange() const {
    if (configuration.find("player_range") != configuration.end()) {
        const std::string& range = configuration.at("player_range");

        std::string cleanedRange = range;
        cleanedRange.erase(std::remove_if(cleanedRange.begin(), cleanedRange.end(), [](char c) {
            return c == '(' || c == ')' || c == ' ';
        }), cleanedRange.end());

        size_t commaPos = cleanedRange.find(',');

        if (commaPos != std::string::npos) {
            try {
                int min = std::stoi(cleanedRange.substr(0, commaPos));
                int max = std::stoi(cleanedRange.substr(commaPos + 1));
                return {min, max};
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument when converting range to integers: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range error when converting range to integers: " << e.what() << std::endl;
            }
        }
    }
    return {0, 0};  
}



bool GameConfig::hasAudience() const {
    if (configuration.find("audience") != configuration.end()) {
        const std::string& audience = configuration.at("audience");
        return audience == "true";
    }
    return false;
}

std::map<std::string, std::string> GameConfig::getConfiguration(){
    return configuration;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> GameConfig::getConstants(){
    return constants;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> GameConfig::getVariables(){
    return variables;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> GameConfig::getPerPlayer(){
    return perPlayer;
}

std::map<std::string, std::vector<std::map<std::string, std::string>>> GameConfig::getSetup(){
    return setup;
}

std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> GameConfig::getPerAudience() {
    return perAudience;
}

void GameConfig::extractStringValue(const ts::Node& node, const std::string& source, 
    std::pair<std::string, std::string>& str1, std::pair<std::string, std::string>& str2, std::string keyID, 
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>& output) {
    
    if (!node.getNumNamedChildren()) {
        auto type = std::string(node.getType());
        if (find(begin(toSkip), end(toSkip), type) == end(toSkip)) {
            auto byteRange = node.getByteRange();
            std::string curr = source.substr(byteRange.start, byteRange.end - byteRange.start);
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
        if (find(begin(toSkip), end(toSkip), type) != end(toSkip)) {
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
void GameConfig::parseValueMap(const ts::Node& node, const std::string& source, T& outputMap) {
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

void GameConfig::setupHelper(const ts::Node& node, const std::string& source, std::string& str1, std::string& str2, const std::string& keyID) {
    if (node.getType() == "number_range") {
        std::string rangeStr;
        for (size_t i = 0; i < node.getNumChildren(); ++i) {
            ts::Node child = node.getChild(i);
            if (child.getType() == "number") {
                auto byteRange = child.getByteRange();
                std::string res = source.substr(byteRange.start, byteRange.end - byteRange.start);
                rangeStr.empty() ? rangeStr = res : rangeStr += ", " + res;
            }
        }
        str2 = rangeStr;
        std::map<std::string, std::string> tempMap;
        tempMap[str1] = str2;
        setup[keyID].push_back(tempMap);
        str1.clear();
        str2.clear();
        return;  
    }

    if (node.getNumChildren() == 0 && (find(begin(toSkip), end(toSkip), std::string(node.getType())) == end(toSkip))) {
        auto byteRange = node.getByteRange();
        std::string res = source.substr(byteRange.start, byteRange.end - byteRange.start);
        if (!res.empty()) {
            size_t pos = res.find(":");
            if (pos != std::string::npos && pos == res.size() - 1) {
                res.erase(pos);
            }
        }
        str1.empty() ? str1 = res : str2 = res;
    }

    for (size_t i = 0; i < node.getNumChildren(); ++i) {
        ts::Node child = node.getChild(i);
        setupHelper(child, source, str1, str2, keyID);
    }

    if (!str1.empty() && !str2.empty()) {
        std::map<std::string, std::string> tempMap;
        tempMap[str1] = str2;
        setup[keyID].push_back(tempMap);
        str1.clear();
        str2.clear();
    }
}

void GameConfig::parsePerPlayerSection(const ts::Node& node, const std::string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, perPlayer);
}

void GameConfig::parsePerAudienceSection(const ts::Node& node, const std::string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, perAudience);   
}

void GameConfig::parseConfigurationSection(const ts::Node& node, const std::string& source) {
    ts::Node nameNode = node.getChildByFieldName("name");
    if (!nameNode.isNull()) {
        auto byteRange = nameNode.getChild(1).getByteRange();
        configuration["name"] = std::string(source.substr(byteRange.start, byteRange.end - byteRange.start));
    }

    ts::Node playerRangeNode = node.getChildByFieldName("player_range");
    if (!playerRangeNode.isNull()) {
        // Use getByteRange to extract the correct part of the source string
        auto byteRange = playerRangeNode.getByteRange();
        std::string extractedRange = source.substr(byteRange.start, byteRange.end - byteRange.start);

        // Store the extracted range into the configuration map
        configuration["player_range"] = extractedRange;
    }

    ts::Node audienceNode = node.getChildByFieldName("has_audience");
    if (!audienceNode.isNull()) {
        auto byteRange = audienceNode.getByteRange();
        configuration["audience"] = std::string(source.substr(byteRange.start, byteRange.end - byteRange.start));
    }

    ts::Node setupNode = node.getChild(10); // set_rule
    ts::Node nestedNode = setupNode.getChildByFieldName("name");
    if (!nestedNode.isNull()) {
        auto byteRange = nestedNode.getByteRange();
        auto key = std::string(source.substr(byteRange.start, byteRange.end - byteRange.start));

        std::string str1 = "";
        std::string str2 = "";
        ts::Cursor cursor = nestedNode.getNextSibling().getCursor();
        auto curr = cursor.getCurrentNode();
        for(size_t i = 0; i < setupNode.getNumChildren() - 1; ++i){
            setupHelper(curr, source, str1, str2, key);
            if(!curr.isNull()){
                curr = curr.getNextSibling();
            }
        }
    }
}


void GameConfig::parseConstantsSection(const ts::Node& node, const std::string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, constants);
}

void GameConfig::parseVariablesSection(const ts::Node& node, const std::string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, variables);
}

void GameConfig::parseConfig(const std::string& fileContent) {
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};
    ts::Tree tree = parser.parseString(fileContent);
    ts::Node root = tree.getRootNode();

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
        }
    }
}


void GameConfig::printKeyValuePair() {
    for (const auto& [key, entries] : constants) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            std::cout << std::endl;
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

void GameConfig::printMap() {
    for (const auto& [key, entries] : configuration) {
        std::cout << key << " : " << entries << std::endl;
    }
}

