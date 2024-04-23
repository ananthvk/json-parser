#include "gtest/gtest.h"
#include "jsonparser.hpp"

TEST(JsonParser, Empty)
{
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}