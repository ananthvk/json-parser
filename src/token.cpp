#include "token.hpp"

Token::Token() : type(Token::Type::UNKNOWN), is_value_present(false) {}

int64_t &Token::as_integer() { return std::get<int64_t>(value); }

long double &Token::as_real() { return std::get<long double>(value); }

std::string &Token::as_string() { return std::get<std::string>(value); }

// Prints the token to std::cout for debugging
void Token::debug()
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
