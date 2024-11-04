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

using PairOfPairs = std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>;
using DataType = std::variant< std::string, bool, PairOfPairs,  std::vector<PairOfPairs>, std::unordered_map<std::string, std::string> >;


class DataValue {
public:
    using SimpleValueType = std::variant<std::string, int, bool>;
    using OrderedMapType = std::vector<std::pair<std::string, DataValue>>;
    using EnumDescriptionType = std::map<std::string, std::string>;
    using ValueType = std::variant<
        std::string,
        int,
        bool,
        std::vector<DataValue>,
        OrderedMapType,
        EnumDescriptionType,
        std::pair<int, int>
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

    // Accessors for each type
    const std::string& asString() const { return std::get<std::string>(value); }
    int asNumber() const { return std::get<int>(value); }
    bool asBoolean() const { return std::get<bool>(value); }
    const std::vector<DataValue>& asList() const { return std::get<std::vector<DataValue>>(value); }
    const OrderedMapType& asOrderedMap() const { return std::get<OrderedMapType>(value); }
    const EnumDescriptionType& asEnumDescription() const { return std::get<EnumDescriptionType>(value); }
    const std::pair<int, int>& asRange() const { return std::get<std::pair<int, int>>(value); }

    const ValueType& getValue() const { return value; }

    std::string getType() const {
        if (std::holds_alternative<std::string>(value)) return "STRING";
        if (std::holds_alternative<int>(value)) return "NUMBER";
        if (std::holds_alternative<bool>(value)) return "BOOLEAN";
        if (std::holds_alternative<std::vector<DataValue>>(value)) return "LIST";
        if (std::holds_alternative<OrderedMapType>(value)) return "ORDERED_MAP";
        if (std::holds_alternative<EnumDescriptionType>(value)) return "ENUM_DESCRIPTION";
        if (std::holds_alternative<std::pair<int, int>>(value)) return "RANGE";
        return "UNKNOWN";
    }

private:
    ValueType value;
};