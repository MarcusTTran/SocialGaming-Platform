#pragma once

#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <variant>

/*
    This is the general variant type class
*/
// TODO: Add error Value
class DataValue {
    

public:
    enum RuleStatus{
        DONE,
        NOTDONE
    }

    using OrderedMapType = std::vector<std::pair<std::string, DataValue>>;
    using EnumDescriptionType = std::map<std::string, DataValue>; 
    using ValueType = std::variant<
        std::string,
        int,
        bool,
        std::vector<DataValue>,
        OrderedMapType,
        EnumDescriptionType,  // For example, look at project description.
        std::pair<int, int>,
        RuleStatus,
    >;

    DataValue() : value("") {}
    explicit DataValue(ValueType v) : value(std::move(v)) {}

    // Overload assignment operators
    DataValue& operator=(bool v) { value = v; return *this; }
    DataValue& operator=(int v) { value = v; return *this; }
    DataValue& operator=(const std::string& v) { value = v; return *this; }
    DataValue& operator=(std::string&& v) { value = std::move(v); return *this; }
    DataValue& operator=(const std::vector<DataValue>& v) { value = v; return *this; }
    DataValue& operator=(std::vector<DataValue>&& v) { value = std::move(v); return *this; }
    DataValue& operator=(const OrderedMapType& v) { value = v; return *this; }
    DataValue& operator=(OrderedMapType&& v) { value = std::move(v); return *this; }
    DataValue& operator=(const EnumDescriptionType& v) { value = v; return *this; }
    DataValue& operator=(EnumDescriptionType&& v) { value = std::move(v); return *this; }
    DataValue& operator=(const std::pair<int, int>& v) { value = v; return *this; }
    DataValue& operator=(std::pair<int, int>&& v) { value = std::move(v); return *this; }
    DataValue& operator=(const RuleStatus & v) { value = v; return *this; }

    // Accessors for each type
    const std::string& asString() const { return std::get<std::string>(value); }
    int asNumber() const { return std::get<int>(value); }
    bool asBoolean() const { return std::get<bool>(value); }
    std::vector<DataValue>& asList() const { return std::get<std::vector<DataValue>>(value); }
    const OrderedMapType& asOrderedMap() const { return std::get<OrderedMapType>(value); }
    const EnumDescriptionType& asEnumDescription() const { return std::get<EnumDescriptionType>(value); }
    const std::pair<int, int>& asRange() const { return std::get<std::pair<int, int>>(value); }
    const RuleStatus& asRuleStatus() const { return std::get<RuleStatus>(value); }

    const ValueType& getValue() const { return value; }

    std::string getType() const {
        if (std::holds_alternative<std::string>(value)) return "STRING";
        if (std::holds_alternative<int>(value)) return "NUMBER";
        if (std::holds_alternative<bool>(value)) return "BOOLEAN";
        if (std::holds_alternative<std::vector<DataValue>>(value)) return "LIST";
        if (std::holds_alternative<OrderedMapType>(value)) return "ORDERED_MAP";
        if (std::holds_alternative<EnumDescriptionType>(value)) return "ENUM_DESCRIPTION";
        if (std::holds_alternative<std::pair<int, int>>(value)) return "RANGE";
        if (std::holds_alternative<RuleStatus>(value)) return "RULE_STATUS";
        return "UNKNOWN";
    }

    // Might not need this, mainly for debugging
    std::ostream& operator<<(std::ostream& os, const RUleStatus& ruleStatus) {
        switch (status) {
            case DONE:
                os << "DONE";
                break;
            case NOTDONE:
                os << "NOTDONE";
                break;
            default:
                os << "UNKNOWN";
                break;
        }
        return os;
    }

    // Note: this is for teammate testing, will be removed eventually
    void print(int indentLevel = 0) const {
        std::string indent(indentLevel, ' '); 
        if (std::holds_alternative<std::string>(value)) {
            std::cout << indent << "\"" << asString() << "\"" << std::endl;
        } else if (std::holds_alternative<int>(value)) {
            std::cout << indent << asNumber() << std::endl;
        } else if (std::holds_alternative<bool>(value)) {
            std::cout << indent << (asBoolean() ? "true" : "false") << std::endl;
        } else if (std::holds_alternative<std::vector<DataValue>>(value)) {
            std::cout << indent << "[\n";
            for (const auto& item : asList()) {
                item.print(indentLevel + 2); 
            }
            std::cout << indent << "]" << std::endl;
        } else if (std::holds_alternative<OrderedMapType>(value)) {
            std::cout << indent << "{\n";
            for (const auto& [key, subValue] : asOrderedMap()) {
                std::cout << indent << "  \"" << key << "\": ";
                subValue.print(indentLevel + 2); 
            }
            std::cout << indent << "}" << std::endl;
        } else if (std::holds_alternative<EnumDescriptionType>(value)) {
            std::cout << indent << "{\n";
            for (const auto& [key, val] : asEnumDescription()) {
                std::cout << indent << "  \"" << key << "\": \"" << val << "\"" << std::endl;
            }
            std::cout << indent << "}" << std::endl;
        } else if (std::holds_alternative<std::pair<int, int>>(value)) {
            auto range = asRange();
            std::cout << indent << "(" << range.first << ", " << range.second << ")" << std::endl;
        } else if (std::holds_alternative<RuleStatus>(value)) {
            std::cout << indent << "(" << asRuleStatus() << ")" << std::endl;
        } 
        
        else {
            std::cout << indent << "UNKNOWN" << std::endl;
        }
    }

private:
    ValueType value;
};