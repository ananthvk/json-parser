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

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}