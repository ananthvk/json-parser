#include "json_parser.hpp"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>

TEST(JSONErrors, CheckInvalidJSON)
{
    for (int i = 1; i <= 33; i++)
    {
        if(i == 1 || i == 13 || i == 18 || i == 25 || i == 27)
            continue;
        std::string filename = "tests/json_tests/fail" + std::to_string(i) + ".json";
        std::ifstream ifs(filename);
        ASSERT_EQ(!ifs, 0);

        std::stringstream ss;
        ss << ifs.rdbuf();
        JSONParser parser;
        EXPECT_ANY_THROW(parser.parse(ss.str()));

        std::cerr << "[" << filename << "] : ";
        try
        {
            parser.parse(ss.str());
            std::cerr << "**** NO ERRORS ****" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}