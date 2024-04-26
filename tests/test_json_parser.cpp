#include "json_parser.hpp"
#include "gtest/gtest.h"

TEST(JSONParser, Empty)
{
    JSONParser parser;
    EXPECT_THROW(parser.parse(""), json_parse_error);

    parser.parse("{}");
    auto tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::OBJECT);
    ASSERT_EQ(tree.size(), 0);

    parser.parse("[]");
    tree = parser.get_tree();
    ASSERT_EQ(tree.size(), 0);
    ASSERT_EQ(tree.type, JSONObjectType::ARRAY);
}

TEST(JSONParser, SimpleValues)
{
    JSONParser parser(R"( {"key": "value", "name": "hello", "address": "world"} )");
    auto tree = parser.get_tree();
    ASSERT_EQ(tree.size(), 3);
    ASSERT_EQ(tree.type, JSONObjectType::OBJECT);
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

    parser.parse(" null ");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::NULL_VALUE);

    parser.parse(R"( {"key": null } )");
    tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::OBJECT);
    ASSERT_EQ(tree["key"].type, JSONObjectType::NULL_VALUE);
}

TEST(JSONParser, KeyValuePairs)
{
    JSONParser parser;
    parser.parse(
        R"( {"a": "string value", "b": 3.1415, "c": true, "d": false, "e": null, "f": 881454} )");
    auto tree = parser.get_tree();

    ASSERT_EQ(tree.type, JSONObjectType::OBJECT);
    ASSERT_EQ(tree.size(), 6);

    ASSERT_EQ(tree["a"].as_string(), "string value");
    ASSERT_NEAR(tree["b"].as_real(), 3.1415, 1e-5);
    ASSERT_EQ(tree["c"].as_bool(), true);
    ASSERT_EQ(tree["d"].as_bool(), false);
    ASSERT_EQ(tree["e"].type, JSONObjectType::NULL_VALUE);
    ASSERT_EQ(tree["f"].as_integer(), 881454);
}

TEST(JSONParser, NestedObjects)
{
    JSONParser parser;
    parser.parse(R"( {"a": {"b":{ "c": { "d": { "key": "this is a value" } } } } } )");
    auto tree = parser.get_tree();
    ASSERT_EQ(tree["a"]["b"]["c"]["d"]["key"].as_string(), "this is a value");
}

TEST(JSONParser, Arrays)
{
    JSONParser parser;
    parser.parse(R"( [1, 2, 3, 4, 5] )");
    auto tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::ARRAY);
    ASSERT_EQ(tree.size(), 5);
    auto v = tree.as_vector();
    for (int i = 1; i <= 5; i++)
    {
        ASSERT_EQ(v[i - 1].as_integer(), i);
    }
}

TEST(JSONParser, MixedArrays)
{
    JSONParser parser;
    parser.parse(R"( [1, 2.314, true, false, null, {}, [], "this is"] )");
    auto tree = parser.get_tree();
    ASSERT_EQ(tree.type, JSONObjectType::ARRAY);
    ASSERT_EQ(tree.size(), 8);
    auto v = tree.as_vector();
    ASSERT_EQ(v[0].type, JSONObjectType::NUMBER_INT);
    ASSERT_EQ(v[1].type, JSONObjectType::NUMBER_REAL);
    ASSERT_EQ(v[2].type, JSONObjectType::BOOLEAN);
    ASSERT_EQ(v[3].type, JSONObjectType::BOOLEAN);
    ASSERT_EQ(v[4].type, JSONObjectType::NULL_VALUE);
    ASSERT_EQ(v[5].type, JSONObjectType::OBJECT);
    ASSERT_EQ(v[6].type, JSONObjectType::ARRAY);
    ASSERT_EQ(v[7].type, JSONObjectType::STRING);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}