#include "json_parser.hpp"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>

TEST(JSONErrors, CheckInvalidJSON)
{
    for (int i = 1; i <= 33; i++)
    {
        std::string filename = "tests/json_tests/fail" + std::to_string(i) + ".json";
        std::ifstream ifs(filename);
        if (!ifs)
        {
            std::cout << "[" << filename << "] : Could not open file" << std::endl;
            continue;
        }
        std::stringstream ss;
        ss << ifs.rdbuf();
        JSONParser parser;
        //EXPECT_THROW(parser.parse(ss.str()), json_parse_error);
        std::cout << "[" << filename << "] : ";
        try
        {
            parser.parse(ss.str());
            std::cout << "**** NO ERRORS ****" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}