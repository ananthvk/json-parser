#pragma once
#include "json_lexer.hpp"
#include "json_object.hpp"
#include <stack>

// TODO: Improve error messages later

class JSONParser
{
    JSONObject root;
    JSONLexer lexer;
    std::stack<Token> tokens;

    // Returns the token to be processed
    Token next();

    // Returns the next token in the stream of tokens, (after the next() token) Token peek();
    Token peek();

    JSONObject parse_value();

    std::pair<std::string, JSONObject> parse_pair();

    std::vector<std::pair<std::string, JSONObject>> parse_pairs();

    JSONObject parse_object();

    std::vector<JSONObject> parse_elements();

    JSONObject parse_array();

  public:
    JSONParser();

    JSONParser(const std::string &buffer);

    void parse();

    void parse(const std::string &buffer);

    JSONObject &get_tree();
};