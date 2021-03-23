#include "gtest/gtest.h"

extern "C" {
    #include "single.h"
}

TEST(test1, test1) {
    ASSERT_EQ(1, 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}