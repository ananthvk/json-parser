#pragma once
#include "json_exceptions.hpp"
#include <map>
#include <stdint.h>
#include <string>
#include <variant>
#include <vector>
enum class JSONObjectType : uint8_t
{
    EMPTY = 0,
    NUMBER_REAL = 1,
    NUMBER_INT = 2,
    STRING = 3,
    BOOLEAN = 4,
    NULL_VALUE = 5,
    OBJECT = 6,
    ARRAY = 7,
};

struct JSONObject
{
    JSONObjectType type;
    std::variant<std::string, long double, int64_t, std::map<std::string, JSONObject>,
                 std::vector<JSONObject>, bool>
        value;

    JSONObject &operator[](const std::string &s)
    {
        if (type != JSONObjectType::OBJECT)
            throw json_access_error();
        return as_kv_pairs()[s];
    }

    JSONObject() : type(JSONObjectType::OBJECT), value(std::map<std::string, JSONObject>()) {}

    JSONObject(JSONObjectType type) : type(type)
    {
        switch (type)
        {
        case JSONObjectType::NUMBER_REAL:
            value = 0.0;
            break;
        case JSONObjectType::NUMBER_INT:
            value = 0;
            break;
        case JSONObjectType::STRING:
            value = "";
            break;
        case JSONObjectType::BOOLEAN:
            value = false;
            break;
        case JSONObjectType::OBJECT:
            value = std::map<std::string, JSONObject>();
            break;
        case JSONObjectType::ARRAY:
            value = std::vector<JSONObject>();
            break;
        default:
            break;
        }
    }

    JSONObject(int64_t val) : type(JSONObjectType::NUMBER_INT), value(val) {}

    JSONObject(long double val) : type(JSONObjectType::NUMBER_REAL), value(val) {}

    JSONObject(const std::string &val) : type(JSONObjectType::STRING), value(val) {}

    JSONObject(const std::vector<JSONObject> &val) : type(JSONObjectType::ARRAY), value(val) {}

    JSONObject(bool val) : type(JSONObjectType::BOOLEAN), value(val) {}

    int64_t &as_integer() { return std::get<int64_t>(value); }

    bool &as_bool() { return std::get<bool>(value); }

    long double &as_real() { return std::get<long double>(value); }

    std::vector<JSONObject> &as_vector() { return std::get<std::vector<JSONObject>>(value); }

    std::string &as_string() { return std::get<std::string>(value); }

    std::map<std::string, JSONObject> &as_kv_pairs()
    {
        return std::get<std::map<std::string, JSONObject>>(value);
    }

    size_t size()
    {
        if (type == JSONObjectType::OBJECT)
            return as_kv_pairs().size();
        if (type == JSONObjectType::ARRAY)
            return as_vector().size();
        return 1;
    }
};