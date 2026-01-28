#include <gtest/gtest.h>

TEST(MyGitBasicTest, CheckAssertions) {
  EXPECT_TRUE(true);
  EXPECT_EQ(2 + 2, 4);
}

TEST(MyGitBasicTest, CheckAssertions2) {
  EXPECT_EQ(2 + 2, 4);
}
