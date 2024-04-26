#pragma once
#include "json_exceptions.hpp"
#include "token.hpp"

// https://www.rfc-editor.org/rfc/rfc8259.txt
// https://www.json.org/json-en.html


// This class converts a sequence of characters into tokens, which can be consumed by the parser.
// The data to be parsed is stored in a string variable buffer, and idx stores the index of
// the character to be processed.
// The main purpose of this class is to group logically related characters into tokens, which can then be parsed.
// These details are abstracted by the methods symbol(), advance() and available()
class JSONLexer
{
    std::string buffer;

    size_t idx;

    char symbol();

    void advance();

    bool available();

    void skip_whitespace();

    bool is_stop();

    Token lex_single_symbol_token();

    Token lex_string();

    Token lex_number();

    Token lex_literal();


  public:
    JSONLexer();

    JSONLexer(const std::string &buffer);

    Token next();

    void load(const std::string &s);

    bool is_next();
};
