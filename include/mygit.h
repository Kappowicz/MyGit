#pragma once
#include <filesystem>
#include <vector>
#include <expected>

struct FileProperties;

void MyGitHelp();

void MyGitInit();

void initDirectory(const std::filesystem::path &directoryPath);

void initFile(const std::filesystem::path &filePath);

std::string calculateHash(const std::filesystem::path &filePath);

void MyGitAdd(const std::vector<std::string_view> &arguments);

void addToIndex(const std::string &fileName, const std::string &hash);

void MyGitCommit(const std::string &message);

void MyGitErase();

void MyGitStatus();

void compareHeadAndIndex(const std::vector<FileProperties> &filesFromIndex);

std::vector<FileProperties> getMyGitFiles(std::ifstream &file);

void MyGitLog();

void MyGitCheckout(const std::string &commitName,
                   bool isSwitchingBranch = false);

void writeToHead(const std::string &message);

std::string getFromHead();

std::string MyGitHashObject(const std::string &filename);

void MyGitDiff();

void MyGitBranch();

void MyGitBranch(const std::string &branchName);

void MyGitSwitch(const std::string &branchName);
