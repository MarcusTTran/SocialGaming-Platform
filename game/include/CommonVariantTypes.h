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

    using OrderedMapType = std::vector<std::pair<std::string, DataValue>>;
    using EnumDescriptionType = std::map<std::string, DataValue>;
    using ValueType = std::variant<std::string, int, bool, std::vector<DataValue>, OrderedMapType,
                                   EnumDescriptionType, // For example, look at project description.
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
    DataValue &operator=(const OrderedMapType &v) {
        value = v;
        return *this;
    }
    DataValue &operator=(OrderedMapType &&v) {
        value = std::move(v);
        return *this;
    }
    DataValue &operator=(const EnumDescriptionType &v) {
        value = v;
        return *this;
    }
    DataValue &operator=(EnumDescriptionType &&v) {
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
    const std::vector<DataValue> &asList() const { return std::get<std::vector<DataValue>>(value); }
    const OrderedMapType &asOrderedMap() const { return std::get<OrderedMapType>(value); }
    const EnumDescriptionType &asEnumDescription() const { return std::get<EnumDescriptionType>(value); }
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
        if (std::holds_alternative<OrderedMapType>(value))
            return "ORDERED_MAP";
        if (std::holds_alternative<EnumDescriptionType>(value))
            return "ENUM_DESCRIPTION";
        if (std::holds_alternative<std::pair<int, int>>(value))
            return "RANGE";
        if (std::holds_alternative<RuleStatus>(value))
            return "RULE_STATUS";
        return "UNKNOWN";
    }

    // // Friend function to overload the << operator for DataValue
    // friend std::ostream &operator<<(std::ostream &os, const DataValue &dataValue) {
    //     if (std::holds_alternative<std::string>(dataValue.value)) {
    //         os << std::get<std::string>(dataValue.value);
    //     } else if (std::holds_alternative<int>(dataValue.value)) {
    //         os << std::get<int>(dataValue.value);
    //     } else if (std::holds_alternative<bool>(dataValue.value)) {
    //         os << (std::get<bool>(dataValue.value) ? "true" : "false");
    //     } else if (std::holds_alternative<std::vector<DataValue>>(dataValue.value)) {
    //         os << "[";
    //         const auto &list = std::get<std::vector<DataValue>>(dataValue.value);
    //         for (size_t i = 0; i < list.size(); ++i) {
    //             os << list[i];
    //             if (i < list.size() - 1) {
    //                 os << ", ";
    //             }
    //         }
    //         os << "]";
    //     } else if (std::holds_alternative<OrderedMapType>(dataValue.value)) {
    //         os << "{";
    //         const auto &map = std::get<OrderedMapType>(dataValue.value);
    //         for (size_t i = 0; i < map.size(); ++i) {
    //             os << "\"" << map[i].first << "\": " << map[i].second;
    //             if (i < map.size() - 1) {
    //                 os << ", ";
    //             }
    //         }
    //         os << "}";
    //     } else if (std::holds_alternative<EnumDescriptionType>(dataValue.value)) {
    //         os << "{";
    //         const auto &enumDesc = std::get<EnumDescriptionType>(dataValue.value);
    //         for (auto it = enumDesc.begin(); it != enumDesc.end(); ++it) {
    //             os << "\"" << it->first << "\": " << it->second;
    //             if (std::next(it) != enumDesc.end()) {
    //                 os << ", ";
    //             }
    //         }
    //         os << "}";
    //     } else if (std::holds_alternative<std::pair<int, int>>(dataValue.value)) {
    //         const auto &pair = std::get<std::pair<int, int>>(dataValue.value);
    //         os << "(" << pair.first << ", " << pair.second << ")";
    //     } else if (std::holds_alternative<RuleStatus>(dataValue.value)) {
    //         os << (std::get<RuleStatus>(dataValue.value) == DONE ? "DONE" : "NOTDONE");
    //     }
    //     return os;
    // }

    // Might not need this, mainly for debugging
    friend std::ostream &operator<<(std::ostream &os, const RuleStatus &ruleStatus) {
        switch (ruleStatus) {
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
    void print(std::ostream& os, int indentLevel = 0) const {
        std::string indent(indentLevel, ' ');
        if (std::holds_alternative<std::string>(value)) {
            os << indent << "\"" << asString() << "\"";
        } else if (std::holds_alternative<int>(value)) {
            os << indent << asNumber();
        } else if (std::holds_alternative<bool>(value)) {
            os << indent << (asBoolean() ? "true" : "false");
        } else if (std::holds_alternative<std::vector<DataValue>>(value)) {
            std::cout << indent << "[\n";
            for (const auto &item : asList()) {
                item.print(os, indentLevel + 2);
            }
            os << indent << "]";
        } else if (std::holds_alternative<OrderedMapType>(value)) {
            std::cout << indent << "{\n";
            for (const auto &[key, subValue] : asOrderedMap()) {
                std::cout << indent << "  \"" << key << "\": ";
                subValue.print(os, indentLevel + 2);
            }
            os << indent << "}";
        } else if (std::holds_alternative<EnumDescriptionType>(value)) {
            std::cout << indent << "{\n";
            for (const auto &[key, val] : asEnumDescription()) {
                std::cout << indent << "  \"" << key << "\": ";
                val.print(os, indentLevel + 2);
            }
            os << indent << "}";
        } else if (std::holds_alternative<std::pair<int, int>>(value)) {
            auto range = asRange();
            os << indent << "(" << range.first << ", " << range.second << ")";
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

// for debugging, it will be removed eventually
inline std::ostream& operator<<(std::ostream& os, const DataValue& data) {
    data.print(os);
    return os;
}
