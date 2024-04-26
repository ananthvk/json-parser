#include "json_object.hpp"

JSONObject &JSONObject::operator[](const std::string &s)
{
    if (type != JSONObjectType::OBJECT)
        throw json_access_error();
    return as_kv_pairs()[s];
}

JSONObject::JSONObject() : type(JSONObjectType::OBJECT), value(std::map<std::string, JSONObject>())
{
}

JSONObject::JSONObject(JSONObjectType type) : type(type)
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

JSONObject::JSONObject(int64_t val) : type(JSONObjectType::NUMBER_INT), value(val) {}

JSONObject::JSONObject(long double val) : type(JSONObjectType::NUMBER_REAL), value(val) {}

JSONObject::JSONObject(const std::string &val) : type(JSONObjectType::STRING), value(val) {}

JSONObject::JSONObject(const std::vector<JSONObject> &val) : type(JSONObjectType::ARRAY), value(val)
{
}

JSONObject::JSONObject(bool val) : type(JSONObjectType::BOOLEAN), value(val) {}

int64_t &JSONObject::as_integer() { return std::get<int64_t>(value); }

bool &JSONObject::as_bool() { return std::get<bool>(value); }

long double &JSONObject::as_real() { return std::get<long double>(value); }

std::vector<JSONObject> &JSONObject::as_vector()
{
    return std::get<std::vector<JSONObject>>(value);
}

std::string &JSONObject::as_string() { return std::get<std::string>(value); }

std::map<std::string, JSONObject> &JSONObject::as_kv_pairs()
{
    return std::get<std::map<std::string, JSONObject>>(value);
}

size_t JSONObject::size()
{
    if (type == JSONObjectType::OBJECT)
        return as_kv_pairs().size();
    if (type == JSONObjectType::ARRAY)
        return as_vector().size();
    return 1;
}