#include "json_lexer.hpp"

char JSONLexer::symbol() { return buffer[idx]; }

void JSONLexer::advance() { idx++; }

bool JSONLexer::available() { return idx < buffer.size(); }

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

bool JSONLexer::is_stop()
{
    return symbol() == ' ' || symbol() == '\n' || symbol() == '\t' || symbol() == '\r' ||
           symbol() == '}' || symbol() == ']' || symbol() == ',';
}

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
    // If the current symbol was a valid token, advance to the next symbol
    if (token.type != Token::Type::UNKNOWN)
        advance();
    return token;
}

Token JSONLexer::lex_string()
{
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

        token.as_string().push_back(symbol());
        advance();
    }
    // Reached end of input without finding matching "
    throw json_parse_error("Unterminated string literal");
}

Token JSONLexer::lex_number()
{
    Token t;
    std::string number;
    bool decimal_point_found = false;
    bool e_found = false;

    if (symbol() == '-')
    {
        number.push_back('-');
        advance();
    }
    if (!isdigit(symbol()))
        return t;
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
    if (decimal_point_found || e_found)
    {
        if(number.back() == 'e' || number.back() == 'E' || number.back() == '+' || number.back() == '-')
            throw json_parse_error("Incomplete number");
        // A real number was found
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

Token JSONLexer::lex_literal()
{
    Token token;
    std::string literal;
    // Check for null, true and false
    while (available() && !is_stop())
    {
        literal.push_back(symbol());
        advance();
    }
    if (literal == "null")
        token.type = Token::Type::LITERAL_NULL;
    else if (literal == "true")
        token.type = Token::Type::LITERAL_TRUE;
    else if (literal == "false")
        token.type = Token::Type::LITERAL_FALSE;
    else
        throw json_parse_error(std::string("Invalid literal \"") + std::string(literal) +
                               std::string("\" in json"));
    return token;
}

JSONLexer::JSONLexer() : idx(0) {}

JSONLexer::JSONLexer(const std::string &buffer) : buffer(buffer), idx(0) {}

Token JSONLexer::next()
{
    // If we have reached the end of the buffer, throw an error
    if (!is_next())
        throw json_lexer_empty_error();

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

void JSONLexer::load(const std::string &s)
{
    buffer = s;
    idx = 0;
}

bool JSONLexer::is_next()
{
    skip_whitespace();
    return idx < buffer.size();
}
