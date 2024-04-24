#include "jsonparser.hpp"
#include "gtest/gtest.h"

TEST(JSONLexer, Empty)
{
    JSONLexer lexer("");
    ASSERT_EQ(lexer.is_next(), false);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load("            ");
    ASSERT_EQ(lexer.is_next(), false);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load(" \n \n\n    \t\t\r    \r\n\n\n        \n    ");
    ASSERT_EQ(lexer.is_next(), false);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);
}

TEST(JSONLexer, SingleSymbolToken)
{
    JSONLexer lexer;

    lexer.load("{[:,]}");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_SQUARE);
    ASSERT_EQ(lexer.next().type, Token::Type::COLON);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);
    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_SQUARE);
    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_BRACE);
    ASSERT_EQ(lexer.is_next(), false);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load("            {      \n       \t     \r    [\n\t\r      :            ,      ]     \n "
               "  \r} \t      ");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_SQUARE);
    ASSERT_EQ(lexer.next().type, Token::Type::COLON);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);
    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_SQUARE);
    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_BRACE);

    lexer.load("{");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);
}

TEST(JSONLexer, SimpleStrings)
{
    JSONLexer lexer;
    lexer.load("\"hello world\"");
    auto token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "hello world");

    // String with letters
    lexer.load("\"hello\"");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "hello");

    // Single character strings
    lexer.load("\"h\"");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "h");

    // Test empty strings
    lexer.load("\"\"");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "");
    ASSERT_EQ(token.as_string().size(), 0);

    // Test that white spaces do not get removed within strings
    lexer.load("                          \n\n\n\r\t        \"\n\nTwo newlines\n\n\rOne\r\"     "
               "\n\n\t\r      ");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "\n\nTwo newlines\n\n\rOne\r");
}

TEST(JSONLexer, UnterminatedString)
{
    JSONLexer lexer;
    lexer.load("\"");
    ASSERT_EQ(lexer.is_next(), true);
    EXPECT_THROW(lexer.next(), json_parse_error);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load("\"Some unterminated content here....");
    ASSERT_EQ(lexer.is_next(), true);
    EXPECT_THROW(lexer.next(), json_parse_error);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load("{,\"this");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);
    EXPECT_THROW(lexer.next(), json_parse_error);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);
}

TEST(JSONLexer, StringAlongWithOtherTokens)
{
    JSONLexer lexer;
    lexer.load(R"(   {"key" : "value"}   )");

    auto token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::LEFT_BRACE);

    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "key");

    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::COLON);

    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "value");

    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::RIGHT_BRACE);
}

TEST(JSONLexer, StringWithUnicodeChars)
{
    JSONLexer lexer;
    lexer.load(R"(  "😁" )");
    auto token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.as_string(), "😁");
}

TEST(JSONLexer, StringWithEscapeCharacters)
{
    JSONLexer lexer;

    struct s
    {
        std::string s1;
        std::string s2;
    };

    std::vector<s> escaped = {
        {R"( "\"" )", "\""}, {R"( "\\" )", "\\"}, {R"( "\/" )", "/"},  {R"( "\b" )", "\b"},
        {R"( "\f" )", "\f"}, {R"( "\n" )", "\n"}, {R"( "\r" )", "\r"}, {R"( "\t" )", "\t"},
    };
    for (auto &e : escaped)
    {
        lexer.load(e.s1);
        auto token = lexer.next();
        ASSERT_EQ(token.type, Token::Type::STRING);
        ASSERT_EQ(token.as_string(), e.s2);
    }
}

TEST(JSONLexer, StringWithEscapeCombined)
{
    JSONLexer lexer;
    lexer.load(R"( "\"\\" )");

    auto token = lexer.next();
    ASSERT_EQ(token.as_string(), "\"\\");

    lexer.load(R"(     "\"\\\/\b\f\n\r\t"    )");
    token = lexer.next();
    ASSERT_EQ(token.as_string(), "\"\\/\b\f\n\r\t");

    lexer.load(R"(     "\nThis is line one\nThis is line two\nThis is line three\n"    )");
    token = lexer.next();
    ASSERT_EQ(token.as_string(), "\nThis is line one\nThis is line two\nThis is line three\n");
}

TEST(JSONLexer, StringWithEscapeCharactersError)
{
    JSONLexer lexer;
    lexer.load(R"( "This is some\ka " )");
    EXPECT_THROW(lexer.next(), json_parse_error);

    lexer.load(R"( "This is some\")");
    EXPECT_THROW(lexer.next(), json_parse_error);

    lexer.load(R"( "This is some\\\")");
    EXPECT_THROW(lexer.next(), json_parse_error);

    lexer.load(R"(     "\"\\\/\b\f\n\r\t\"    )");
    EXPECT_THROW(lexer.next(), json_parse_error);
}

TEST(JSONLexer, SampleJSON)
{
    JSONLexer lexer;
    lexer.load(R"( {"name" : "Ginger", "type" : "cat", "likes": ["toys", "food"]} )");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);

    ASSERT_EQ(lexer.next().type, Token::Type::STRING);
    ASSERT_EQ(lexer.next().type, Token::Type::COLON);
    ASSERT_EQ(lexer.next().type, Token::Type::STRING);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);

    ASSERT_EQ(lexer.next().type, Token::Type::STRING);
    ASSERT_EQ(lexer.next().type, Token::Type::COLON);
    ASSERT_EQ(lexer.next().type, Token::Type::STRING);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);

    ASSERT_EQ(lexer.next().type, Token::Type::STRING);
    ASSERT_EQ(lexer.next().type, Token::Type::COLON);

    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_SQUARE);
    ASSERT_EQ(lexer.next().type, Token::Type::STRING);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);
    ASSERT_EQ(lexer.next().type, Token::Type::STRING);
    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_SQUARE);

    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_BRACE);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}