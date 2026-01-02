#pragma once
#include "mygit.h"
#include <filesystem>
#include <iomanip>

struct FileProperties;

void printHelp();

void initMyGit();

std::string calculateHash(const std::string &fileName);

void MyGitAdd(const std::string &fileName);

void addToIndex(const std::string &fileName, const std::string &hash);

void MyGitCommit(const std::string &message);

void MyGitErase();

void MyGitStatus();

void compareHeadAndIndex(std::vector<FileProperties> &filesFromIndex);

std::vector<FileProperties> getMyGitFiles(std::ifstream &file);

void MyGitLog();
