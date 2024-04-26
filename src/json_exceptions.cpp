#include "json_exceptions.hpp"

json_not_implemented_error::json_not_implemented_error()
    : message("This feature has not yet been implemented")
{
}

json_not_implemented_error::json_not_implemented_error(const std::string &message)
    : message(message)
{
}

const char *json_not_implemented_error::what() const noexcept { return message.c_str(); }

json_parse_error::json_parse_error() : message("Error parsing JSON") {}

json_parse_error::json_parse_error(const std::string &message) : message(message) {}

json_parse_error::json_parse_error(const std::string &message, const Token &tok)
    : message(message + tok.as_exception_string())
{
}

const char *json_parse_error::what() const noexcept { return message.c_str(); }

json_access_error::json_access_error() : message("Invalid access") {}

json_access_error::json_access_error(const std::string &message) : message(message) {}

const char *json_access_error::what() const noexcept { return message.c_str(); }