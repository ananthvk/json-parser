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

TEST(JSONParser, IntegrationTest)
{
    JSONParser parser;
    parser.parse(R"(
        {
            "productName" : "Dell Latitude 7490",
            "productClass" : "Laptop",
            "productPrice": 100.003,
            "productId" : 3,
            "available": true,
            "additionalDetails": null,
            "featureIds": [1, 3, 8, 7, 21, 15],
            "dimension": {
                "width": 13.03,
                "depth": 8.70
            },
            "cpuSpecs": {
                "manufacturer": "intel",
                "model" : "i5-8350U",
                "features": [2, 3, 8, 12, 16, 84]
            }
        }
    )");
    auto tree = parser.get_tree();
    ASSERT_EQ(tree["productName"].as_string(), "Dell Latitude 7490");
    ASSERT_EQ(tree["productClass"].as_string(), "Laptop");
    ASSERT_NEAR(tree["productPrice"].as_real(), 100.003, 1e-5);
    ASSERT_EQ(tree["productId"].as_integer(), 3);
    ASSERT_EQ(tree["available"].as_bool(), true);
    ASSERT_EQ(tree["additionalDetails"].type, JSONObjectType::NULL_VALUE);

    ASSERT_EQ(tree["featureIds"].type, JSONObjectType::ARRAY);
    auto vals = tree["featureIds"].as_vector();
    
    ASSERT_EQ(vals[0].as_integer(), 1);
    ASSERT_EQ(vals[1].as_integer(), 3);
    ASSERT_EQ(vals[2].as_integer(), 8);
    ASSERT_EQ(vals[3].as_integer(), 7);
    ASSERT_EQ(vals[4].as_integer(), 21);
    ASSERT_EQ(vals[5].as_integer(), 15);

    
    auto subtree = tree["dimension"];
    ASSERT_EQ(subtree.type, JSONObjectType::OBJECT);
    ASSERT_EQ(subtree.size(), 2);
    ASSERT_NEAR(subtree["width"].as_real(), 13.03, 1e-5);
    ASSERT_NEAR(subtree["depth"].as_real(), 8.70, 1e-5);

    subtree = tree["cpuSpecs"];
    ASSERT_EQ(subtree["manufacturer"].as_string(), "intel");
    ASSERT_EQ(subtree["model"].as_string(), "i5-8350U");

    ASSERT_EQ(subtree["features"].type, JSONObjectType::ARRAY);
    vals = subtree["features"].as_vector();
    
    ASSERT_EQ(vals[0].as_integer(), 2);
    ASSERT_EQ(vals[1].as_integer(), 3);
    ASSERT_EQ(vals[2].as_integer(), 8);
    ASSERT_EQ(vals[3].as_integer(), 12);
    ASSERT_EQ(vals[4].as_integer(), 16);
    ASSERT_EQ(vals[5].as_integer(), 84);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}