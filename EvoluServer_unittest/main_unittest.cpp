#include <iostream>
#include "gtest/gtest.h"
#include "spmath.h"

TEST(testMath, myCubeTest)
{
	EXPECT_EQ(1000, cubic(10));
}