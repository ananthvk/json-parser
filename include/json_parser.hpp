#pragma once
#include "json_lexer.hpp"
#include <map>
#include <stack>
#include <vector>

enum class JSONObjectType : uint8_t
{
    EMPTY = 0,
    NUMBER_REAL = 1,
    NUMBER_INT = 2,
    STRING = 3,
    BOOLEAN = 4,
    NULL_VALUE = 5,
    OBJECT = 6,
    ARRAY = 7,
};

struct JSONObject
{
    JSONObjectType type;
    std::variant<std::string, long double, int64_t, std::map<std::string, JSONObject>,
                 std::vector<JSONObject>, bool>
        value;

    JSONObject &operator[](const std::string &s)
    {
        if (type != JSONObjectType::OBJECT)
            throw json_access_error();
        return as_kv_pairs()[s];
    }

    JSONObject() : type(JSONObjectType::OBJECT), value(std::map<std::string, JSONObject>()) {}

    JSONObject(JSONObjectType type) : type(type) {}

    JSONObject(int64_t val) : type(JSONObjectType::NUMBER_INT), value(val) {}

    JSONObject(long double val) : type(JSONObjectType::NUMBER_REAL), value(val) {}

    JSONObject(const std::string &val) : type(JSONObjectType::STRING), value(val) {}

    JSONObject(bool val) : type(JSONObjectType::BOOLEAN), value(val) {}

    int64_t &as_integer() { return std::get<int64_t>(value); }

    bool &as_bool() { return std::get<bool>(value); }

    long double &as_real() { return std::get<long double>(value); }

    std::vector<JSONObject> &as_vector() { return std::get<std::vector<JSONObject>>(value); }

    std::string &as_string() { return std::get<std::string>(value); }

    std::map<std::string, JSONObject> &as_kv_pairs()
    {
        return std::get<std::map<std::string, JSONObject>>(value);
    }

    size_t size()
    {
        if (type == JSONObjectType::OBJECT)
            return as_kv_pairs().size();
        if (type == JSONObjectType::ARRAY)
            return as_vector().size();
        return 1;
    }
};

class JSONParser
{
    JSONObject root;
    JSONLexer lexer;
    std::stack<Token> tokens;

    // Returns the token to be processed
    Token next()
    {
        // If no tokens are waiting to be processed, get the next token
        if (tokens.empty())
            return lexer.next();

        Token top = tokens.top();
        tokens.pop();
        return top;
    }

    // Returns the next token in the stream of tokens, (after the next() token)
    Token peek()
    {
        if (tokens.empty())
        {
            Token token = lexer.next();
            tokens.push(token);
            return token;
        }
        return tokens.top();
    }

    JSONObject value()
    {
        auto token = next();
        switch (token.type)
        {
        case Token::Type::STRING:
            return JSONObject(token.as_string());
            break;
        case Token::Type::NUMBER_INTEGER:
            return JSONObject(token.as_integer());
            break;
        case Token::Type::NUMBER_REAL:
            return JSONObject(token.as_real());
            break;
        case Token::Type::LEFT_BRACE:
            // TODO: Process an object
            break;
        case Token::Type::LEFT_SQUARE:
            // TODO: Process an array
            break;
        case Token::Type::LITERAL_TRUE:
            return JSONObject(true);
            break;
        case Token::Type::LITERAL_FALSE:
            return JSONObject(false);
            break;
        case Token::Type::LITERAL_NULL:
            return JSONObject(JSONObjectType::NULL_VALUE);
            break;
        default:
            throw json_parse_error("Expected value");
        }
        throw json_parse_error("Expected value");
    }

  public:
    JSONParser() {}

    JSONParser(const std::string &buffer) : lexer(buffer) { parse(); }

    void parse()
    {
        // value = string | number | object | array | "true" | "false" | "null"
        // pair = string ":" value
        // pairs = pair | (pair "," pairs)
        // elements = value | (value "," elements)
        // array = "[" elements "]" | "[" "]"
        // object = "{" pairs "}" | "{" "}"
        // json = array | object | value
        root = value();
    }

    void parse(const std::string &buffer)
    {
        lexer.load(buffer);
        parse();
    }

    JSONObject &get_tree() { return root; }
};