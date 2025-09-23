#include <iostream>
#include "mygit.h"
#include <filesystem> //available from c++17
#include <fstream>
#include <string>

const std::string MAIN_FOLDER_NAME = ".mygit";
const std::string OBJECTS_FOLDER_NAME = "/objects";
const std::string INDEX_FILE_NAME = "/index";

void printHelp() {
  std::cout << "this is my simple implementation of git \n start by initializing the repo with ./mygit init " <<
      std::endl;
}

void initMyGit() {
  if (std::filesystem::is_directory(MAIN_FOLDER_NAME)) {
    std::cout << "folder " << MAIN_FOLDER_NAME << " exists" << std::endl;
  } else {
    if (std::filesystem::create_directory(MAIN_FOLDER_NAME)) {
      std::cout << "folder " << MAIN_FOLDER_NAME << " created" << std::endl;
    } else
      std::cout << "folder " << MAIN_FOLDER_NAME << " failed to create" << std::endl;
  }

  if (std::filesystem::create_directory(MAIN_FOLDER_NAME + OBJECTS_FOLDER_NAME)) {
    std::cout << "folder " << MAIN_FOLDER_NAME + OBJECTS_FOLDER_NAME << " created" << std::endl;
  } else
    std::cout << "folder " << MAIN_FOLDER_NAME + OBJECTS_FOLDER_NAME << " failed to create" << std::endl;
}

std::string calculateHash(const std::string &fileName) //something like djb2
{
  std::ifstream file(fileName, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + fileName);
  }

  unsigned long long count = 1;
  char c;
  while (file.get(c)) {
    count = ((count << 2) + count) + c;
  }

  std::cout << std::endl << "Hash for " << fileName << ": " << count << std::endl;

  return std::to_string(count);
}

void MyGitAdd(const std::string &fileName) {
  std::string hash = calculateHash(fileName);
  std::string fileDestination = MAIN_FOLDER_NAME + OBJECTS_FOLDER_NAME + "/" + hash;
  if (std::filesystem::exists(fileDestination)) {
    std::cout << "File " << fileDestination << " already exists" << std::endl;
    return;
  }
  std::filesystem::copy_file(fileName, fileDestination);

  addToIndex(fileName, hash);
}

void addToIndex(const std::string &fileName, std::string hash) {
  std::ofstream file(MAIN_FOLDER_NAME + INDEX_FILE_NAME);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + fileName);
  }
  std::filesystem::path filePath(fileName);
  std::filesystem::file_status status = std::filesystem::status(filePath);
  std::filesystem::perms permisions = status.permissions();

  std::string output = hash + " " + fileName;

  file.write(output.c_str(), output.size());
}
