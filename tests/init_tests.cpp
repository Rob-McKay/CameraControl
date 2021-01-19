#include "gtest/gtest.h"
#include <iostream>

TEST(Init, test){
    std::cout << "g" << std::endl;
    EXPECT_EQ(1, 1);
}
