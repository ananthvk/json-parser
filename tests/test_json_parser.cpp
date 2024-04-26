#include "json_parser.hpp"
#include "gtest/gtest.h"

TEST(JSONParser, Empty)
{
    //JSONParser parser;

    //EXPECT_THROW(parser.parse(""), json_lexer_empty_error);

    //parser.parse("{}");
    //auto tree = parser.get_tree();
    //ASSERT_EQ(tree.size(), 0);

    // parser.parse("[]");
    // tree = parser.get_tree();
    // ASSERT_EQ(tree.size(), 0);
}

TEST(JSONParser, SimpleValues)
{
    JSONParser parser(R"( {"key": "value", "name": "hello", "address": "world"} )");
    auto tree = parser.get_tree();
    ASSERT_EQ(tree.size(), 3);
    ASSERT_EQ(tree["key"].as_string(), "value");
    ASSERT_EQ(tree["name"].as_string(), "hello");
    ASSERT_EQ(tree["address"].as_string(), "world");

    parser.parse(R"( "hello world" )");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::STRING);
    ASSERT_EQ(tree.as_string(), "hello world");

    parser.parse(" 3.1415 ");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::NUMBER_REAL);
    ASSERT_NEAR(3.1415, tree.as_real(), 1e-5);

    parser.parse(" -32124 ");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::NUMBER_INT);
    ASSERT_EQ(tree.as_integer(), -32124);

    parser.parse(" true ");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::BOOLEAN);
    ASSERT_EQ(tree.as_bool(), true);

    parser.parse(" false ");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::BOOLEAN);
    ASSERT_EQ(tree.as_bool(), false);

    parser.parse(" null { ");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::NULL_VALUE);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}