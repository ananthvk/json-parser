#pragma once
#include "json_lexer.hpp"
#include "json_object.hpp"
#include <stack>

/*
 * This class implements the parser logic for parsing JSON.
 * It contains a JSONObject root, which represents the root of the parsed tree, and a Lexer object
 * which is used to obtain tokens from the input string. This parser is a recursive descent parser.
 * TODO: Improve error messages, also add an option to specify recursion depth
*/
class JSONParser
{
    JSONObject root;
    JSONLexer lexer;
    std::stack<Token> tokens;

    Token next();

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