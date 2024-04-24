#include "jsonparser.hpp"
#include "gtest/gtest.h"

TEST(JSONLexer, Empty)
{
    JSONLexer lexer("");
    ASSERT_EQ(lexer.is_next(), false);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load("            ");
    ASSERT_EQ(lexer.is_next(), true);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load(" \n \n\n    \t\t\r    \r\n\n\n        \n    ");
    ASSERT_EQ(lexer.is_next(), true);
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
    ASSERT_EQ(token.string_value, "hello world");

    // String with letters
    lexer.load("\"hello\"");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.string_value, "hello");

    // Single character strings
    lexer.load("\"h\"");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.string_value, "h");

    // Test empty strings
    lexer.load("\"\"");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.string_value, "");
    ASSERT_EQ(token.string_value.size(), 0);

    // Test that white spaces do not get removed within strings
    lexer.load("                          \n\n\n\r\t        \"\n\nTwo newlines\n\n\rOne\r\"     \n\n\t\r      ");
    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.string_value, "\n\nTwo newlines\n\n\rOne\r");
}

TEST(JSONLexer, UnterminatedString)
{
    JSONLexer lexer;
    lexer.load("\"");
    ASSERT_EQ(lexer.is_next(), true);
    EXPECT_THROW(lexer.next(), json_lexer_unterminated_string);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);

    lexer.load("\"Some unterminated content here....");
    ASSERT_EQ(lexer.is_next(), true);
    EXPECT_THROW(lexer.next(), json_lexer_unterminated_string);
    EXPECT_THROW(lexer.next(), json_lexer_empty_error);
    
    lexer.load("{,\"this");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);
    EXPECT_THROW(lexer.next(), json_lexer_unterminated_string);
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
    ASSERT_EQ(token.string_value, "key");

    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::COLON);

    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::STRING);
    ASSERT_EQ(token.string_value, "value");

    token = lexer.next();
    ASSERT_EQ(token.type, Token::Type::RIGHT_BRACE);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}