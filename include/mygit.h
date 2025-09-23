#pragma once
#include "mygit.h"
#include <filesystem>
#include <iomanip>

void printHelp();

void initMyGit();

std::string calculateHash(const std::string &fileName);

void MyGitAdd(const std::string &fileName);
