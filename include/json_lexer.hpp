#pragma once
#include "token.hpp"
#include "json_exceptions.hpp"

// https://www.rfc-editor.org/rfc/rfc8259.txt
// https://www.json.org/json-en.html


// Scans the input and produces a stream of JSON tokens
class JSONLexer
{
    std::string buffer;

    // idx is the index of the next character to be processed
    size_t idx;

    // Returns the character to be processed
    char symbol();

    // Updates idx to point to the next character
    void advance();

    // Returns true if there are more characters to be processed
    bool available();

    // Skip whitespace characters
    void skip_whitespace();

    bool is_stop();

    // Find a single symbol tokens such as braces, comma, etc
    // Returns the token with type set to the token type if found, otherwise sets it to
    // UNKNOWN, signifying that it may be some other type of token
    Token lex_single_symbol_token();

    // Scans the input for a string, a string begins and terminates with a "
    Token lex_string();

    Token lex_number();

    Token lex_literal();


  public:
    JSONLexer();

    JSONLexer(const std::string &buffer);

    // Returns the next token
    Token next();

    void load(const std::string &s);

    // Returns true if there are more tokens
    bool is_next();
};
