#pragma once
#include <iostream>
#include "mygit.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <iomanip>

void printHelp();
void initMyGit();
std::string calculateHash(std::string fileName);