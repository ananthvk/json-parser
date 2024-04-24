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
        NUMBER,
        BOOLEAN,
        LITERAL_NULL,
    } type;

    std::string string_value;
    int64_t integer_value;
    double real_value;
    bool bool_value;
};

class json_lexer_empty_error : public std::exception
{
    std::string message;

  public:
    json_lexer_empty_error() : message("Cannot parse JSON, since input string is empty") {}

    json_lexer_empty_error(const std::string &message) : message(message) {}

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

    // Get a single character token
    Token get_character_token()
    {
        Token token;
    }


  public:
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
        token.type = Token::Type::BOOLEAN;
        return token;
    }

    void load(const std::string &s)
    {
        buffer = s;
        idx = 0;
    }

    // Returns true if there can be any more tokens
    bool is_next() { return idx < buffer.size(); }
};
