#pragma once
#include <iostream>
#include <string>
#include <stdint.h>
#include <variant>

// A token class represents JSON token
class Token
{
  public:
    // An enum which represents type of the token
    enum class Type : uint8_t
    {
        RIGHT_BRACE,
        LEFT_BRACE,

        RIGHT_SQUARE,
        LEFT_SQUARE,

        COMMA,
        COLON,

        STRING,
        NUMBER_REAL,
        NUMBER_INTEGER,
        LITERAL_TRUE,
        LITERAL_FALSE,
        LITERAL_NULL,

        UNKNOWN
    } type;


    bool is_value_present;

    std::variant<std::string, int64_t, long double> value;

    // Default constructor, which initializes the type to UNKNOWN
    Token();

    int64_t &as_integer();

    long double &as_real();

    std::string &as_string();

    // Prints the token to std::cout for debugging
    void debug();
};
