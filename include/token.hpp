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
    Token() : type(Token::Type::UNKNOWN), is_value_present(false) {}

    int64_t &as_integer() { return std::get<int64_t>(value); }

    long double &as_real() { return std::get<long double>(value); }

    std::string &as_string() { return std::get<std::string>(value); }

    // Prints the token to std::cout for debugging
    void debug()
    {
        std::cout << "Token( ";
        switch (type)
        {
        case Type::RIGHT_BRACE:
            std::cout << "}";
            break;
        case Type::LEFT_BRACE:
            std::cout << "{";
            break;
        case Type::RIGHT_SQUARE:
            std::cout << "]";
            break;
        case Type::LEFT_SQUARE:
            std::cout << "[";
            break;
        case Type::COMMA:
            std::cout << ",";
            break;
        case Type::COLON:
            std::cout << ":";
            break;
        case Type::STRING:
            std::cout << " \"" << as_string() << "\"";
            break;
        case Type::NUMBER_REAL:
            std::cout << as_real();
            break;
        case Type::NUMBER_INTEGER:
            std::cout << as_integer();
            break;
        case Type::LITERAL_TRUE:
            std::cout << "true";
            break;
        case Type::LITERAL_FALSE:
            std::cout << "false";
            break;
        case Type::LITERAL_NULL:
            std::cout << "null";
            break;
        case Type::UNKNOWN:
            std::cout << "UNKNOWN";
            break;
        }
        std::cout << " )";
    }
};
