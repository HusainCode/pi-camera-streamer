#include <gtest/gtest.h>

// Entry point for GoogleTest
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}