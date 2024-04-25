#pragma once
#include <exception>
#include <string>

// Thrown when there are no more characters to process
class json_lexer_empty_error : public std::exception
{
    std::string message;

  public:
    json_lexer_empty_error();

    json_lexer_empty_error(const std::string &message);

    const char *what() const noexcept override;
};

// Thrown for a feature which is not implemented
class json_lexer_not_implemented_error : public std::exception
{
    std::string message;

  public:
    json_lexer_not_implemented_error();

    json_lexer_not_implemented_error(const std::string &message);

    const char *what() const noexcept override;
};

// Thrown for any syntax/lexical error
class json_parse_error : public std::exception
{
    std::string message;

  public:
    json_parse_error();

    json_parse_error(const std::string &message);

    const char *what() const noexcept override;
};