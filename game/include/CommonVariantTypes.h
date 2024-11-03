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
    // Use a simplified type for elements of lists and maps
    using SimpleValueType = std::variant<std::string, int, bool>;

    // A map-like structure with order preservation
    using OrderedMapType = std::vector<std::pair<std::string, DataValue>>;

    using ValueType = std::variant<
        std::string, 
        int, 
        bool, 
        std::vector<DataValue>, 
        OrderedMapType 
    >;

    // Default constructor
    DataValue() : value("") {} 

    explicit DataValue(ValueType v) : value(std::move(v)) {}

    DataValue& operator=(bool v) {
        value = v;
        return *this;
    }

    DataValue& operator=(int v) {
        value = v;
        return *this;
    }

    DataValue& operator=(const std::string& v) {
        value = v;
        return *this;
    }

    DataValue& operator=(std::string&& v) {
        value = std::move(v);
        return *this;
    }

    DataValue& operator=(const std::vector<DataValue>& v) {
        value = v;
        return *this;
    }

    // avoid copying while transfering
    DataValue& operator=(std::vector<DataValue>&& v) {
        value = std::move(v);
        return *this;
    }

    DataValue& operator=(const OrderedMapType& v) {
        value = v;
        return *this;
    }

    DataValue& operator=(OrderedMapType&& v) {
        value = std::move(v);
        return *this;
    }

    const std::string& asString() const { return std::get<std::string>(value); }
    int asNumber() const { return std::get<int>(value); }
    bool asBoolean() const { return std::get<bool>(value); }
    const std::vector<DataValue>& asList() const { return std::get<std::vector<DataValue>>(value); }
    const OrderedMapType& asOrderedMap() const { return std::get<OrderedMapType>(value); }

    const ValueType& getValue() const { return value; }

    std::string getType() const {
        if (std::holds_alternative<std::string>(value)) {
            return "STRING";
        } else if (std::holds_alternative<int>(value)) {
            return "NUMBER";
        } else if (std::holds_alternative<bool>(value)) {
            return "BOOLEAN";
        } else if (std::holds_alternative<std::vector<DataValue>>(value)) {
            return "LIST";
        } else if (std::holds_alternative<OrderedMapType>(value)) {
            return "ORDERED_MAP";
        }
        return "UNKNOWN";
    }

private:
    ValueType value;
};