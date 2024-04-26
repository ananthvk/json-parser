#include "json_lexer.hpp"

/// @brief This method returns the current character(sybmol) being processed
/// @return Returns a character
char JSONLexer::symbol() { return buffer[idx]; }

/// @brief Advances the index to the next character in the sequence
void JSONLexer::advance() { idx++; }

/// @brief Checks if there are any more characters to be processed
/// @return true if there are characters to be processedj
bool JSONLexer::available() { return idx < buffer.size(); }

/// @brief Skips whitespace characters such as newline, tab and space,
/// since these characters do not have any meaning in JSON syntax.
void JSONLexer::skip_whitespace()
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

/// @brief Check if the current character is a boundary character such as a parenthesis.
/// These characters separate literals and values from each other
/// @return true if the character is a boundary character
bool JSONLexer::is_stop()
{
    return symbol() == ' ' || symbol() == '\n' || symbol() == '\t' || symbol() == '\r' ||
           symbol() == '}' || symbol() == ']' || symbol() == ',';
}

/// @brief This function scans the input for single character tokens such as comma, parenthesis,
/// etc.
/// @return A token with type set to UNKNOWN if the character does not represent a single character
/// token, otherwise a token object with type set to the type of token scanned.
Token JSONLexer::lex_single_symbol_token()
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
    // If the current symbol was a valid single character token,
    // discard the current character and move to the next one
    if (token.type != Token::Type::UNKNOWN)
        advance();
    return token;
}

/// @brief This function scans the input for a string
/// A JSON string is a group of characters surrounded by double quotes (").
/// @return A token with type set to UNKNOWN if a string cannot be found, otherwise the scanned
/// string
/// TODO: Implement unicode escape sequence, also check if a character is a control character
Token JSONLexer::lex_string()
{
    Token token;
    // Check if the current character marks the beginning of a string
    if (symbol() != '"')
        return token;

    token.type = Token::Type::STRING;
    // Discard the scanned quote
    advance();

    while (available())
    {
        // Check if the current character represents the start of an escape sequence
        while (available() && symbol() == '\\')
        {
            // Discard the reverse solidus
            advance();

            // There has to be atleast one character after an escape sequence
            if (!available())
                throw json_parse_error("Unterminated string literal");

            if (symbol() == '/')
                token.as_string().push_back('/');

            else if (symbol() == '\\')
                token.as_string().push_back('\\');

            else if (symbol() == '"')
                token.as_string().push_back('"');

            else if (symbol() == 'b')
                token.as_string().push_back('\b');

            else if (symbol() == 'f')
                token.as_string().push_back('\f');

            else if (symbol() == 'n')
                token.as_string().push_back('\n');

            else if (symbol() == 'r')
                token.as_string().push_back('\r');

            else if (symbol() == 't')
                token.as_string().push_back('\t');

            else if (symbol() == 'u')
                throw json_not_implemented_error("Unicode is not yet implemented");

            else
                throw json_parse_error("Invalid escape character");

            advance();
        }
        if (!available())
            throw json_parse_error("Unterminated string literal");

        // Check if the end of the string has been reached
        if (symbol() == '"')
        {
            advance();
            return token;
        }

        // Add the current character to the string
        token.as_string().push_back(symbol());
        advance();
    }
    // Reached end of input without finding matching "
    throw json_parse_error("Unterminated string literal");
}

/// @brief This function scans the input for a number
/// Even though JSON has a single number type, I have implemented two sub types - integer and real
/// numbers in this parser. This is to maintain precision of number and to differentiate between
/// integers and doubles for various uses.
/// TODO: Currently this method does not throw an error when a number begins with 0, fix it later.
/// @return A token with type set to UNKNOWN if a number cannot be found, otherwise the scanned
/// number.
Token JSONLexer::lex_number()
{
    Token t;
    std::string number;

    // These flags are used to ensure that only a single decimal point / e should exist in a number
    bool decimal_point_found = false;
    bool e_found = false;

    // A number can begin with a negative sign, but only at the beginning
    if (symbol() == '-')
    {
        number.push_back('-');
        advance();
    }

    // If the character is not a digit, it is possibly some other token
    if (!isdigit(symbol()))
    {
        // But if a dash (minus sign) has been found, this becomes an invalid token
        if (number.size() > 0)
            throw json_parse_error("Invalid literal \"-\"");
        return t;
    }

    // Stores the previous character, this is needed to check if a minus(-) or (+)
    // appears after an exponent (e/E)
    char last = symbol();

    while (available())
    {
        if (isdigit(symbol()))
        {
            number.push_back(symbol());
        }
        else if ((last == 'e' || last == 'E') && (symbol() == '-' || symbol() == '+'))
        {
            number.push_back(symbol());
        }
        else if (!decimal_point_found && symbol() == '.')
        {
            number.push_back(symbol());
            decimal_point_found = true;
        }
        else if (!e_found && (symbol() == 'e' || symbol() == 'E'))
        {
            number.push_back(symbol());
            e_found = true;
        }
        else if (is_stop())
        {
            break;
        }
        else
        {
            throw json_parse_error(std::string("Invalid literal '") + symbol() +
                                   std::string("' for number"));
        }
        last = symbol();
        advance();
    }
    // Check if the number detected is a real number
    if (decimal_point_found || e_found)
    {
        // Detect cases where e is at the end of the number, e.g. 3e or 3e+
        if (number.back() == 'e' || number.back() == 'E' || number.back() == '+' ||
            number.back() == '-')
            throw json_parse_error("Incomplete number");
        t.type = Token::Type::NUMBER_REAL;
        t.value = std::stold(number);
    }
    else
    {
        t.type = Token::Type::NUMBER_INTEGER;
        t.value = std::stoll(number);
    }
    return t;
}

/// @brief This function scans the input for a literal.
/// There are only three types of literal in JSON - null, true and false.
/// To detect a literal, scan characters until a stop character is encountered
/// @return A token with type set to UNKNOWN if a literal cannot be found, otherwise the scanned
/// literal.
Token JSONLexer::lex_literal()
{
    Token token;
    std::string literal;
    while (available() && !is_stop())
    {
        literal.push_back(symbol());
        advance();
    }

    // Check for null, true and false
    if (literal == "null")
        token.type = Token::Type::LITERAL_NULL;
    else if (literal == "true")
        token.type = Token::Type::LITERAL_TRUE;
    else if (literal == "false")
        token.type = Token::Type::LITERAL_FALSE;
    else
        throw json_parse_error(std::string("Invalid literal \"") + std::string(literal) +
                               std::string("\""));
    return token;
}

/// Default constructor for the lexer, initializes variables to their default values
/// A call to load is needed later to be able to tokenize the input
JSONLexer::JSONLexer() : idx(0) {}

JSONLexer::JSONLexer(const std::string &buffer) : buffer(buffer), idx(0) {}

/// @brief This method detects tokens in the input string.
/// This method scans the input and returns the next token found.
/// An error is raised if the next token cannot be found
/// @return token - Next valid JSON token
Token JSONLexer::next()
{
    // If we have reached the end of the buffer, throw an error
    if (!is_next())
        throw json_parse_error();

    Token token;

    if ((token = lex_single_symbol_token()).type != Token::Type::UNKNOWN)
        return token;

    if ((token = lex_string()).type != Token::Type::UNKNOWN)
        return token;

    if ((token = lex_number()).type != Token::Type::UNKNOWN)
        return token;

    if ((token = lex_literal()).type != Token::Type::UNKNOWN)
        return token;

    else
        throw json_parse_error("Invalid JSON");
}

/// Loads the given input string
void JSONLexer::load(const std::string &s)
{
    buffer = s;
    idx = 0;
}

/// Checks if there are any characters left to be processed
/// @return true if the input is not yet empty
bool JSONLexer::is_next()
{
    skip_whitespace();
    return idx < buffer.size();
}
