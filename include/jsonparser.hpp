#pragma once
#include <exception>
#include <iostream>
#include <stdint.h>
#include <string>

// https://www.rfc-editor.org/rfc/rfc8259.txt
// https://www.json.org/json-en.html


// A token class representing a token in JSON
class Token
{
  public:
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
        BOOLEAN,
        LITERAL_NULL,

        UNKNOWN
    } type;


    bool is_value_present;

    std::string string_value;
    int64_t integer_value;
    double real_value;
    bool bool_value;

    Token() : type(Token::Type::UNKNOWN), is_value_present(false) {}

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
            std::cout << " \"" << string_value << "\"";
            break;
        case Type::NUMBER_REAL:
            std::cout << real_value;
            break;
        case Type::NUMBER_INTEGER:
            std::cout << integer_value;
            break;
        case Type::BOOLEAN:
            std::cout << (bool_value ? "true" : "false");
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

class json_lexer_empty_error : public std::exception
{
    std::string message;

  public:
    json_lexer_empty_error() : message("Cannot parse JSON, since input string is empty") {}

    json_lexer_empty_error(const std::string &message) : message(message) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class json_lexer_not_implemented_error : public std::exception
{
    std::string message;

  public:
    json_lexer_not_implemented_error() : message("This feature has not yet been implemented") {}

    json_lexer_not_implemented_error(const std::string &message) : message(message) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class json_parse_error : public std::exception
{
    std::string message;

  public:
    json_parse_error() : message("This feature has not yet been implemented") {}

    json_parse_error(const std::string &message) : message(message) {}

    const char *what() const noexcept override { return message.c_str(); }
};

// Lexes the input and produces a stream of JSON tokens
class JSONLexer
{
    std::string buffer;

    // idx is the index of the next character to be processed
    size_t idx;

    // Returns the character to be processed
    char symbol() { return buffer[idx]; }

    // Updates idx to point to the next character
    void advance() { idx++; }

    // Returns true if there are more characters to be processed
    bool available() { return idx < buffer.size(); }

    void skip_whitespace()
    {
        while (available())
        {
            switch (symbol())
            {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                advance();
                break;
            default:
                return;
            }
        }
    }

    // Find a single symbol tokens such as braces, comma, etc
    // Returns the token with type set to the token type if found, otherwise sets it to
    // UNKNOWN, signifying that it may be some other type of token
    Token lex_single_symbol_token()
    {
        Token token;
        switch (symbol())
        {
        case '{':
            token.type = Token::Type::LEFT_BRACE;
            break;

        case '}':
            token.type = Token::Type::RIGHT_BRACE;
            break;

        case '[':
            token.type = Token::Type::LEFT_SQUARE;
            break;

        case ']':
            token.type = Token::Type::RIGHT_SQUARE;
            break;

        case ':':
            token.type = Token::Type::COLON;
            break;

        case ',':
            token.type = Token::Type::COMMA;
            break;

        default:
            token.type = Token::Type::UNKNOWN;
            break;
        }
        // If the current symbol was a valid token, advance to the next symbol
        if (token.type != Token::Type::UNKNOWN)
            advance();
        return token;
    }

    Token lex_string()
    {
        // https://en.cppreference.com/w/cpp/language/escape
        // Check start of a string
        Token token;
        if (symbol() != '"')
            // Not a string
            return token;

        token.type = Token::Type::STRING;
        advance();

        while (available())
        {
            // Check for escape sequence
            while (available() && symbol() == '\\')
            {
                advance();
                // There has to be atleast one character after a reverse solidus
                if (!available())
                    throw json_parse_error("Unterminated string literal");

                if (symbol() == '/')
                    token.string_value.push_back('/');

                else if (symbol() == '\\')
                    token.string_value.push_back('\\');

                else if (symbol() == '"')
                    token.string_value.push_back('"');

                else if (symbol() == 'b')
                    token.string_value.push_back('\b');

                else if (symbol() == 'f')
                    token.string_value.push_back('\f');

                else if (symbol() == 'n')
                    token.string_value.push_back('\n');

                else if (symbol() == 'r')
                    token.string_value.push_back('\r');

                else if (symbol() == 't')
                    token.string_value.push_back('\t');

                else if (symbol() == 'u')
                    throw json_lexer_not_implemented_error("Unicode is not yet implemented");

                else
                    throw json_parse_error("Invalid escape character");

                advance();
            }
            if (!available())
                throw json_parse_error("Unterminated string literal");

            // TODO: Implement unicode escape sequence, checking of control characters
            // Found end of string
            if (symbol() == '"')
            {
                advance();
                return token;
            }

            token.string_value.push_back(symbol());
            advance();
        }
        // Reached end of input without finding matching "
        throw json_parse_error("Unterminated string literal");
    }


  public:
    JSONLexer() : idx(0) {}

    JSONLexer(const std::string &buffer) : buffer(buffer), idx(0) {}

    // Returns the next token
    Token next()
    {
        // Skip leading whitespaces
        skip_whitespace();

        // If we have reached the end of the buffer, throw an error
        if (!is_next())
            throw json_lexer_empty_error();

        Token token;

        if ((token = lex_single_symbol_token()).type != Token::Type::UNKNOWN)
            return token;

        if ((token = lex_string()).type != Token::Type::UNKNOWN)
            return token;

        return token;
    }

    void load(const std::string &s)
    {
        buffer = s;
        idx = 0;
    }

    // Returns true if there can be any more tokens
    // Note: If there are trailing whitespaces, this function returns true even though there are no
    // more tokens
    bool is_next() { return idx < buffer.size(); }
};
