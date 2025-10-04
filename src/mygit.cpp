#include <iostream>
#include "mygit.h"
#include <filesystem> //available from c++17
#include <fstream>
#include <string>

const std::string MAIN_FOLDER_NAME = ".mygit";
const std::string OBJECTS_FOLDER_NAME = "/objects";
const std::string INDEX_FILE_NAME = "/index";
const std::string EXEC_CHAR = "e";
const std::string NOT_EXEC_CHAR = "n";

const std::string INDEX_FILE_LOCALIZATION = MAIN_FOLDER_NAME + INDEX_FILE_NAME;
const std::string OBJECTS_FOLDER_LOCALIZATION = MAIN_FOLDER_NAME + OBJECTS_FOLDER_NAME;

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

  if (std::filesystem::create_directory(OBJECTS_FOLDER_LOCALIZATION)) {
    std::cout << "folder " << OBJECTS_FOLDER_LOCALIZATION << " created" << std::endl;
  } else
    std::cout << "folder " << OBJECTS_FOLDER_LOCALIZATION << " failed to create" << std::endl;
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
  std::string fileDestination = OBJECTS_FOLDER_LOCALIZATION + "/" + hash;
  if (std::filesystem::exists(fileDestination)) {
    std::cout << "File " << fileDestination << " already exists" << std::endl;
    return;
  }
  std::filesystem::copy_file(fileName, fileDestination);

  addToIndex(fileName, hash);
}

void addToIndex(const std::string &fileName, std::string hash) {
  std::ofstream file(INDEX_FILE_LOCALIZATION, std::ios::app);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + fileName);
  }
  std::filesystem::path filePath(fileName);
  std::filesystem::file_status status = std::filesystem::status(filePath);
  std::filesystem::perms permisions = status.permissions();
  std::string currExecChar = permisions == std::filesystem::perms::owner_exec ? EXEC_CHAR : NOT_EXEC_CHAR;

  std::string output = currExecChar + " " + hash + " " + fileName + "\n";

  file.write(output.c_str(), output.size());
}

void MyGitCommit() {
  /* sample commit object
tree f314a9254316e1a92a54466b81bdd09415c44136   <- this commit tree hash from index file
parent                                          <- previous commit hash from index file
Initial commit with siema.txt and ok.png        <- this commit's message
   */
  //Create tree object
  std::ifstream file(INDEX_FILE_LOCALIZATION, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + INDEX_FILE_LOCALIZATION);
  }
  std::string word;
  //for now there are only 3 words in each line of index file, exec rights, hash and localization, to update later
  int amountOfWordsInLine = 3;
  int currentWordCount = 3;
  while (file >> word) {
    // displaying content

    if (currentWordCount % amountOfWordsInLine == 0) {
      //Exec rights word
      std::cout << " Exec rights: " << word;
      currentWordCount = 0;
    } else if (currentWordCount % amountOfWordsInLine == 1) {
      std::cout << " Hash: " << word;
    } else if (currentWordCount % amountOfWordsInLine == 2) {
      std::cout << " File path: " << word;
      std::cout << std::endl;
    } else {
      std::cout << " Something different: " << word;
    }

    //std::cout << " " << word;

    currentWordCount++;
  }
}
