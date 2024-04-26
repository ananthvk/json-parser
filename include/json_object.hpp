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

    JSONObject &operator[](const std::string &s);

    JSONObject();

    JSONObject(JSONObjectType type);

    JSONObject(int64_t val);

    JSONObject(long double val);

    JSONObject(const std::string &val);

    JSONObject(const std::vector<JSONObject> &val);

    JSONObject(bool val);

    int64_t &as_integer();

    bool &as_bool();

    long double &as_real();

    std::vector<JSONObject> &as_vector();

    std::string &as_string();

    std::map<std::string, JSONObject> &as_kv_pairs();

    size_t size();
};