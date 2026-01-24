#pragma once
#include "mygit.h"
#include <filesystem>
#include <vector>

struct FileProperties;

void printHelp();

void MyGitInit();

void initDirectory(const std::string &directory);

void initFile(const std::string &fileName);

std::string calculateHash(const std::string &fileName);

void MyGitAdd(const std::string &fileName);

void addToIndex(const std::string &fileName, const std::string &hash);

void MyGitCommit(const std::string &message);

void MyGitErase();

void MyGitStatus();

void compareHeadAndIndex(const std::vector<FileProperties> &filesFromIndex);

std::vector<FileProperties> getMyGitFiles(std::ifstream &file);

void MyGitLog();

void MyGitCheckout(const std::string &commitName, bool isSwitchingBranch = false);

void writeToHead(const std::string &message);

std::string getFromHead();

std::string MyGitHashObject(const std::string &filename);

void MyGitDiff();

void MyGitBranch();

void MyGitBranch(const std::string &branchName);

void MyGitSwitch(const std::string &branchName);
