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

    lexer.load("            {      \n       \t     \r    [\n\t\r      :            ,      ]     \n   \r} \t      ");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_SQUARE);
    ASSERT_EQ(lexer.next().type, Token::Type::COLON);
    ASSERT_EQ(lexer.next().type, Token::Type::COMMA);
    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_SQUARE);
    ASSERT_EQ(lexer.next().type, Token::Type::RIGHT_BRACE);
    
    lexer.load("{");
    ASSERT_EQ(lexer.next().type, Token::Type::LEFT_BRACE);

}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}