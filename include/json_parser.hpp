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

    JSONObject parse_value()
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
            tokens.push(token);
            return parse_object();
            break;
        case Token::Type::LEFT_SQUARE:
            tokens.push(token);
            return parse_array();
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
            throw json_parse_error("Expected value, found ", token);
        }
        throw json_parse_error("Expected value, found ", token);
    }

    std::pair<std::string, JSONObject> parse_pair()
    {
        auto key = next();

        auto separator = next();

        if (separator.type != Token::Type::COLON)
            throw json_parse_error("Invalid key-value pair, expected \":\", found ", separator);

        auto value = parse_value();


        return std::make_pair(key.as_string(), value);
    }

    std::vector<std::pair<std::string, JSONObject>> parse_pairs()
    {
        std::vector<std::pair<std::string, JSONObject>> result;
        result.push_back(parse_pair());
        while (1)
        {
            auto token = peek();
            if (token.type == Token::Type::COMMA)
            {
                // Remove the comma token
                next();
                // Find the next pair
                result.push_back(parse_pair());
            }
            else
            {
                break;
            }
        }
        return result;
    }

    JSONObject parse_object()
    {
        auto token = peek();
        if (token.type != Token::Type::LEFT_BRACE)
        {
            // Not an object
            return JSONObject(JSONObjectType::EMPTY);
        }
        // Remove the left brace
        next();

        token = peek();
        if (token.type == Token::Type::RIGHT_BRACE)
        {
            // This is an empty object
            next();
            return JSONObject();
        }

        auto pairs = parse_pairs();

        // Find the closing brace
        token = peek();
        if (token.type != Token::Type::RIGHT_BRACE)
        {
            throw json_parse_error("Expected \"}\", found ", token);
        }
        next();

        JSONObject ob;
        for (auto &pair : pairs)
        {
            ob[pair.first] = pair.second;
        }
        return ob;
    }

    std::vector<JSONObject> parse_elements()
    {
        std::vector<JSONObject> result;
        result.push_back(parse_value());
        while (1)
        {
            auto token = peek();
            if (token.type == Token::Type::COMMA)
            {
                // Remove the comma token
                next();
                // Find the next element
                result.push_back(parse_value());
            }
            else
            {
                break;
            }
        }
        return result;
    }

    JSONObject parse_array()
    {
        auto token = peek();
        if (token.type != Token::Type::LEFT_SQUARE)
        {
            // Not an array
            return JSONObject(JSONObjectType::EMPTY);
        }
        // Remove the left square parenthesis
        next();

        token = peek();
        if (token.type == Token::Type::RIGHT_SQUARE)
        {
            // This is an empty array
            next();
            return JSONObject(JSONObjectType::ARRAY);
        }

        auto elements = parse_elements();

        // Find the closing parenthesis
        token = peek();
        if (token.type != Token::Type::RIGHT_SQUARE)
        {
            throw json_parse_error("Expected \"]\", found ", token);
        }
        next();

        return JSONObject(elements);
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
        // json = value

        root = parse_value();
    }

    void parse(const std::string &buffer)
    {
        lexer.load(buffer);
        parse();
    }

    JSONObject &get_tree() { return root; }
};