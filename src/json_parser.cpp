#include "json_parser.hpp"

/// @brief  Returns the next token to be processed
/// @return  token
Token JSONParser::next()
{
    // If no tokens are waiting to be processed, get the next token
    if (tokens.empty())
        return lexer.next();

    Token top = tokens.top();
    tokens.pop();
    return top;
}

/// @brief Returns the next token to be processed, but does not consume it 
/// i.e. the next call to next() returns the same token. This is used to implement lookahed in this parser.
/// @return token
Token JSONParser::peek()
{
    if (tokens.empty())
    {
        Token token = lexer.next();
        tokens.push(token);
        return token;
    }
    return tokens.top();
}

/*
 * A value can be  either  a string or a number or an object or an array
 * It can also be the literals - true, false and null
 * @return Parsed value in a JSONObject
 */
JSONObject JSONParser::parse_value()
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

/// Parses a single pair, i.e. <STRING> <COLON> <VALUE>
/// These pairs appear inside objects
std::pair<std::string, JSONObject> JSONParser::parse_pair()
{
    auto key = next();

    auto separator = next();

    if (separator.type != Token::Type::COLON)
        throw json_parse_error("Invalid key-value pair, expected \":\", found ", separator);

    auto value = parse_value();


    return std::make_pair(key.as_string(), value);
}

/// Parses multiple pairs, this is achieved by calling parse_pair() repeatedly
/// when a comma is encountered after parsing a pair.
std::vector<std::pair<std::string, JSONObject>> JSONParser::parse_pairs()
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

/// Parses an object
/// An object in JSON is defined as { <KEY-VALUE PAIRS> } or { }
JSONObject JSONParser::parse_object()
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

/// Elements can either be a single value, or a value followed by a comma, followed by more elements
std::vector<JSONObject> JSONParser::parse_elements()
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

/// Parses a JSON array, represented by [ ] or [ <ELEMENTS> ]
JSONObject JSONParser::parse_array()
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

JSONParser::JSONParser() {}

JSONParser::JSONParser(const std::string &buffer) : lexer(buffer) { parse(); }

/// This method calls parse_value(), which in turn recursively calls the other methods
/// to parse the JSON input. This method should be called for parsing the input buffer.
void JSONParser::parse()
{
    // value = string | number | object | array | "true" | "false" | "null"
    // pair = string ":" value
    // pairs = pair | (pair "," pairs)
    // elements = value | (value "," elements)
    // array = "[" elements "]" | "[" "]"
    // object = "{" pairs "}" | "{" "}"
    // json = value

    root = parse_value();
    if(lexer.is_next())
    {
        // There are more tokens after parsing, these tokens are invalid
        throw json_parse_error("Extra tokens after parsing JSON");
    }
}

void JSONParser::parse(const std::string &buffer)
{
    lexer.load(buffer);
    parse();
}

JSONObject &JSONParser::get_tree() { return root; }
