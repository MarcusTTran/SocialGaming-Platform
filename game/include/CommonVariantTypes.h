#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "Server.h"
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class DataValue {

public:
    enum RuleStatus { DONE, NOTDONE };

    using OrderedMapType = std::map<std::string, DataValue>;  // Swapped
    using EnumDescriptionType = std::vector<std::pair<std::string, DataValue>>;  // Swapped
    using ValueType = std::variant<std::string, int, bool, std::vector<DataValue>, EnumDescriptionType,
                                   OrderedMapType, // Updated positions in variant
                                   std::pair<int, int>, RuleStatus, networking::Connection>;

    // Default constructor
    DataValue() : value("") {}

    // Constructor with a ValueType
    explicit DataValue(ValueType v) : value(std::move(v)) {}

    // Overload assignment operators
    DataValue &operator=(bool v) {
        value = v;
        return *this;
    }
    DataValue &operator=(int v) {
        value = v;
        return *this;
    }
    DataValue &operator=(const std::string &v) {
        value = v;
        return *this;
    }
    DataValue &operator=(std::string &&v) {
        value = std::move(v);
        return *this;
    }
    DataValue &operator=(const std::vector<DataValue> &v) {
        value = v;
        return *this;
    }
    DataValue &operator=(std::vector<DataValue> &&v) {
        value = std::move(v);
        return *this;
    }
    DataValue &operator=(const EnumDescriptionType &v) {  // Updated type
        value = v;
        return *this;
    }
    DataValue &operator=(EnumDescriptionType &&v) {  // Updated type
        value = std::move(v);
        return *this;
    }
    DataValue &operator=(const OrderedMapType &v) {  // Updated type
        value = v;
        return *this;
    }
    DataValue &operator=(OrderedMapType &&v) {  // Updated type
        value = std::move(v);
        return *this;
    }
    DataValue &operator=(const std::pair<int, int> &v) {
        value = v;
        return *this;
    }
    DataValue &operator=(std::pair<int, int> &&v) {
        value = std::move(v);
        return *this;
    }
    DataValue &operator=(const RuleStatus &v) {
        value = v;
        return *this;
    }

    // Accessors for each type
    const std::string &asString() const { return std::get<std::string>(value); }
    int asNumber() const { return std::get<int>(value); }
    bool asBoolean() const { return std::get<bool>(value); }
    std::vector<DataValue> &asList() const { return std::get<std::vector<DataValue>>(value); }
    const OrderedMapType &asOrderedMap() const { return std::get<OrderedMapType>(value); }  // Updated type
    const EnumDescriptionType &asEnumDescription() const { return std::get<EnumDescriptionType>(value); }  // Updated type
    const std::pair<int, int> &asRange() const { return std::get<std::pair<int, int>>(value); }
    const RuleStatus &asRuleStatus() const { return std::get<RuleStatus>(value); }
    const networking::Connection &asConnection() const { return std::get<networking::Connection>(value); }

    const ValueType &getValue() const { return value; }

    // Get the type of value as a string
    std::string getType() const {
        if (std::holds_alternative<std::string>(value))
            return "STRING";
        if (std::holds_alternative<int>(value))
            return "NUMBER";
        if (std::holds_alternative<bool>(value))
            return "BOOLEAN";
        if (std::holds_alternative<std::vector<DataValue>>(value))
            return "LIST";
        if (std::holds_alternative<EnumDescriptionType>(value))  // Updated condition
            return "ENUM_DESCRIPTION";
        if (std::holds_alternative<OrderedMapType>(value))  // Updated condition
            return "ORDERED_MAP";
        if (std::holds_alternative<std::pair<int, int>>(value))
            return "RANGE";
        if (std::holds_alternative<RuleStatus>(value))
            return "RULE_STATUS";
        return "UNKNOWN";
    }

    // Updated `print` function
    void print(std::ostream &os, int indentLevel = 0) const {
        std::string indent(indentLevel, ' ');
        if (std::holds_alternative<std::string>(value)) {
            os << indent << "\"" << asString() << "\"";
        } else if (std::holds_alternative<int>(value)) {
            os << indent << asNumber();
        } else if (std::holds_alternative<bool>(value)) {
            os << indent << (asBoolean() ? "true" : "false");
        } else if (std::holds_alternative<std::vector<DataValue>>(value)) {
            os << indent << "[\n";
            for (const auto &item : asList()) {
                item.print(os, indentLevel + 2);
                os << "\n";
            }
            os << indent << "]";
        } else if (std::holds_alternative<OrderedMapType>(value)) {  // Updated logic
            os << indent << "{\n";
            for (const auto &[key, subValue] : asOrderedMap()) {
                os << indent << "  \"" << key << "\": ";
                subValue.print(os, indentLevel + 2);
                os << "\n";
            }
            os << indent << "}";
        } else if (std::holds_alternative<EnumDescriptionType>(value)) {  // Updated logic
            os << indent << "[\n";
            for (const auto &[key, val] : asEnumDescription()) {
                os << indent << "  {\"" << key << "\": ";
                val.print(os, indentLevel + 2);
                os << "},\n";
            }
            os << indent << "]";
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

// Overloaded `<<` operator
inline std::ostream &operator<<(std::ostream &os, const DataValue &data) {
    data.print(os);
    return os;
}

