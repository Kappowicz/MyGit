#pragma once
#include <filesystem>
#include <vector>
#include <expected>

struct FileProperties;

std::vector<std::string_view> getCloseStrings(const std::string_view &target,
                                              const std::vector<std::string_view> &available);

void MyGitHelp();

void MyGitInit();

void initDirectory(const std::filesystem::path &directoryPath);

void initFile(const std::filesystem::path &filePath);

std::string calculateHash(const std::filesystem::path &filePath);

void MyGitAdd(const std::vector<std::string_view> &arguments);

void addToIndex(const std::filesystem::path &filePathToAdd, const std::string &hash);

void MyGitCommit(const std::vector<std::string_view> &arguments);

void MyGitErase();

void MyGitStatus();

void compareHeadAndIndex(const std::vector<FileProperties> &filesFromIndex);

std::vector<FileProperties> getMyGitFiles(std::ifstream &file);

void MyGitLog();

void MyGitCheckout(const std::string &commitName, bool isSwitchingBranch = false);

void writeToHead(const std::string &message);

std::string getFromHead();

void MyGitHashObject(const std::vector<std::string_view> &arguments);

void MyGitDiff();

void MyGitBranch();

void MyGitBranch(const std::vector<std::string_view> &arguments);

void MyGitSwitch(const std::vector<std::string_view> &arguments);
