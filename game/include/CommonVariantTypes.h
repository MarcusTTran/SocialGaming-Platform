#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <variant>

class DataValue {
public:
    enum class RuleStatus {
        DONE,
        NOTDONE
    };

    using OrderedMapType = std::vector<std::pair<std::string, DataValue>>;
    using EnumDescriptionType = std::map<std::string, DataValue>;
    using ValueType = std::variant<
        std::string,
        int,
        bool,
        std::vector<DataValue>,
        OrderedMapType,
        EnumDescriptionType,
        std::pair<int, int>,
        RuleStatus
    >;

    // Default constructor
    DataValue() : value("") {}

    // Constructor with a ValueType
    explicit DataValue(ValueType v) : value(std::move(v)) {}

    // Assignment operators
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
    DataValue& operator=(const RuleStatus& v) { value = v; return *this; }

    // Accessor methods for each type
    const std::string& asString() const { return std::get<std::string>(value); }
    int asNumber() const { return std::get<int>(value); }
    bool asBoolean() const { return std::get<bool>(value); }
    const std::vector<DataValue>& asList() const { return std::get<std::vector<DataValue>>(value); }
    const OrderedMapType& asOrderedMap() const { return std::get<OrderedMapType>(value); }
    const EnumDescriptionType& asEnumDescription() const { return std::get<EnumDescriptionType>(value); }
    const std::pair<int, int>& asRange() const { return std::get<std::pair<int, int>>(value); }
    const RuleStatus& asRuleStatus() const { return std::get<RuleStatus>(value); }

    const ValueType& getValue() const { return value; }

    // Get the type of value as a string
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

    // Print function for debugging
    void print(std::ostream& os, int indentLevel = 0) const {
        std::string indent(indentLevel, ' ');
        if (std::holds_alternative<std::string>(value)) {
            os << indent << "\"" << asString() << "\"";
        } else if (std::holds_alternative<int>(value)) {
            os << indent << asNumber();
        } else if (std::holds_alternative<bool>(value)) {
            os << indent << (asBoolean() ? "true" : "false");
        } else if (std::holds_alternative<std::vector<DataValue>>(value)) {
            os << indent << "[\n";
            for (const auto& item : asList()) {
                item.print(os, indentLevel + 2);
                os << ",\n";
            }
            os << indent << "]";
        } else if (std::holds_alternative<OrderedMapType>(value)) {
            os << indent << "{\n";
            for (const auto& [key, subValue] : asOrderedMap()) {
                os << indent << "  \"" << key << "\": ";
                subValue.print(os, indentLevel + 2);
                os << "\n";
            }
            os << indent << "}";
        } else if (std::holds_alternative<EnumDescriptionType>(value)) {
            os << indent << "{\n";
            for (const auto& [key, val] : asEnumDescription()) {
                os << indent << "  \"" << key << "\": ";
                val.print(os, indentLevel + 2);
                os << "\n";
            }
            os << indent << "}";
        } else if (std::holds_alternative<std::pair<int, int>>(value)) {
            auto range = asRange();
            os << indent << "(" << range.first << ", " << range.second << ")";
        } else if (std::holds_alternative<RuleStatus>(value)) {
            os << indent << (asRuleStatus() == RuleStatus::DONE ? "DONE" : "NOTDONE");
        } else {
            os << indent << "UNKNOWN";
        }
    }

private:
    ValueType value;
};

// for debugging, it will be removed eventually
inline std::ostream& operator<<(std::ostream& os, const DataValue& data) {
    data.print(os);
    return os;
}
