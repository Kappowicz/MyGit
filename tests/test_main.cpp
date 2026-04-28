#include <gtest/gtest.h>
#include "../include/mygit.h"
#include <filesystem>
namespace fs = std::filesystem;

TEST(MyGitBasicTest, TemplateAssertion) {
  EXPECT_TRUE(true);
  EXPECT_EQ(2 + 2, 4);
}

TEST(MyGitBasicTest, TestMyGitInit) {
  MyGitInit();
  EXPECT_TRUE(fs::exists(".mygit"));
  EXPECT_TRUE(fs::is_directory(".mygit"));

  fs::remove_all(".mygit");
}

