#pragma once
#include "mygit.h"
#include <filesystem>
#include <iomanip>

struct FileProperties;

void printHelp();

void initMyGit();

std::string calculateHash(const std::string &fileName);

void MyGitAdd(const std::string &fileName);

void addToIndex(const std::string &fileName, std::string hash);

void MyGitCommit(std::string message);

void MyGitErase();

void MyGitStatus();

std::vector<FileProperties> getMyGitFiles(std::ifstream &file);
