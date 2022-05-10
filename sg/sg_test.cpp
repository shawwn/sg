#include "sg.hpp"
#include "gtest/gtest.h"
GTEST_TEST(SGTest, Basic) { ASSERT_EQ(true, true); }
// GTEST_TEST(SGTest, Basic2) { ASSERT_EQ(false, true); }
GTEST_TEST(SGTest, Basic3) { std::cout << "Foo\n"; }
GTEST_TEST(SGTest, Basic4) { std::cout << "Bar\n"; }
// GTEST_TEST(SGTest, test_sg) { sg::test_sg(); }
