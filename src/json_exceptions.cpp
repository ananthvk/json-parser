#include "json_exceptions.hpp"

json_lexer_empty_error::json_lexer_empty_error()
    : message("Cannot parse JSON, since input string is empty")
{
}

json_lexer_empty_error::json_lexer_empty_error(const std::string &message) : message(message) {}

const char *json_lexer_empty_error::what() const noexcept { return message.c_str(); }

json_lexer_not_implemented_error::json_lexer_not_implemented_error()
    : message("This feature has not yet been implemented")
{
}

json_lexer_not_implemented_error::json_lexer_not_implemented_error(const std::string &message)
    : message(message)
{
}

const char *json_lexer_not_implemented_error::what() const noexcept { return message.c_str(); }

json_parse_error::json_parse_error() : message("This feature has not yet been implemented") {}

json_parse_error::json_parse_error(const std::string &message) : message(message) {}

const char *json_parse_error::what() const noexcept { return message.c_str(); }