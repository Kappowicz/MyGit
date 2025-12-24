#include <iostream>
#include "mygit.h"
#include <filesystem> //available from c++17
#include <fstream>
#include <string>
#include <sstream>

const std::string MAIN_FOLDER_NAME = ".mygit";
const std::string OBJECTS_FOLDER_NAME = "/objects";
const std::string REFS_FOLDER_NAME = "/refs";
const std::string INDEX_FILE_NAME = "/index";
const std::string TEMP_COMMIT_FILE_NAME = "/.tempcommit";
const std::string EXEC_CHAR = "e";
const std::string NOT_EXEC_CHAR = "n";
const std::string MAIN_BRANCH_NAME = "main";

const std::string INDEX_FILE_LOCALIZATION = MAIN_FOLDER_NAME + INDEX_FILE_NAME;
const std::string OBJECTS_FOLDER_LOCALIZATION = MAIN_FOLDER_NAME + OBJECTS_FOLDER_NAME;
const std::string TEMP_COMMIT_FILE_LOCALIZATION = MAIN_FOLDER_NAME + TEMP_COMMIT_FILE_NAME;

const std::string REFS_FOLDER_LOCALIZATION = MAIN_FOLDER_NAME + REFS_FOLDER_NAME;
const std::string MAIN_BRANCH_LOCALIZATION = REFS_FOLDER_LOCALIZATION + "/" + MAIN_BRANCH_NAME;

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

  if (std::filesystem::create_directory(REFS_FOLDER_LOCALIZATION)) {
    std::cout << "folder " << REFS_FOLDER_LOCALIZATION << " created" << std::endl;
  } else
    std::cout << "folder " << REFS_FOLDER_LOCALIZATION << " failed to create" << std::endl;

  std::ofstream file(MAIN_BRANCH_LOCALIZATION, std::ios::app);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + MAIN_BRANCH_LOCALIZATION);
  } else {
    std::cout << "created empty file for default branch: " << MAIN_BRANCH_LOCALIZATION << std::endl;
  }
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

  //std::cout << "Hash for " << fileName << ": " << count << std::endl;

  return std::to_string(count);
}

void MyGitAdd(const std::string &fileName) {
  if (!std::filesystem::exists(MAIN_FOLDER_NAME)) {
    std::cout << "Folder " << MAIN_FOLDER_NAME << " doesn't exist!" << std::endl;
    std::cout << "Maybe you didn't './MyGit init'?" << std::endl;
    return;
  }
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
    throw std::runtime_error("Failed to open the file2: " + INDEX_FILE_LOCALIZATION);
  }
  std::ifstream file2(INDEX_FILE_LOCALIZATION);
  if (!file2.is_open()) {
    std::cout << "index file doesnt exist!" << std::endl;
    return;
  }

  std::vector<std::pair<std::string, std::string> > foundFiles = getMyGitFiles(file2);
  bool fileFound = false;
  for (auto const &[filePath, fileHash]: foundFiles) {
    if (filePath == fileName) {
      std::cout << "File name: " << filePath << " already exists!" << std::endl;
      fileFound = true;
    }
  }

  std::ofstream tempFile(INDEX_FILE_LOCALIZATION + ".temp");
  if (!tempFile.is_open()) {
    throw std::runtime_error("Failed to open the file2: " + INDEX_FILE_LOCALIZATION);
  }

  if (fileFound) {
    std::string output;
    //change only the hash of the file
    for (auto const &[filePath, fileHash]: foundFiles) {
      if (filePath == fileName) {
        //todo: make generateFiles function get also exec rights from file, so for now we will leave -
        output = "file - " + calculateHash(filePath) + " " + filePath + "\n";
        tempFile.write(output.c_str(), output.size());
      } else {
        output = "file - " + fileHash + " " + filePath + "\n";
        tempFile.write(output.c_str(), output.size());
      }
    }
    tempFile.close();
    std::filesystem::copy_file(INDEX_FILE_LOCALIZATION + ".temp",
                               INDEX_FILE_LOCALIZATION, std::filesystem::copy_options::overwrite_existing);
  } else {
    //write at the end of the file
    std::filesystem::path filePath(fileName);
    std::filesystem::file_status status = std::filesystem::status(filePath);
    std::filesystem::perms permisions = status.permissions();
    std::string currExecChar =
        (permisions & std::filesystem::perms::owner_exec) != std::filesystem::perms::none
          ? EXEC_CHAR
          : NOT_EXEC_CHAR;
    std::string output = "file " + currExecChar + " " + hash + " " + fileName + "\n";
    file.write(output.c_str(), output.size());
  }
}

void MyGitCommit(std::string message) {
  /* sample commit object
tree f314a9254316e1a92a54466b81bdd09415c44136   <- this commit tree hash from index file
parent                                          <- previous commit hash from index file (empty for first commit)
Initial commit with siema.txt and ok.png        <- this commit's message
   */
  if (!std::filesystem::exists(INDEX_FILE_LOCALIZATION)) {
    std::cout << "File " << INDEX_FILE_LOCALIZATION << " doesn't exist!" << std::endl;
    std::cout << "Maybe you didn't './MyGit add' any files after './MyGit init'?" << std::endl;
    return;
  }
  std::ifstream file(INDEX_FILE_LOCALIZATION, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + INDEX_FILE_LOCALIZATION);
  }
  std::ofstream tempCommitFile(TEMP_COMMIT_FILE_LOCALIZATION, std::ios::binary);
  if (!tempCommitFile.is_open()) {
    throw std::runtime_error("Failed to open the file: " + TEMP_COMMIT_FILE_LOCALIZATION);
  }

  std::cout << "Commit's message: " << message << std::endl;
  tempCommitFile << "message\t" << message << std::endl;

  //get current time in nanoseconds
  auto systemClockNow = std::chrono::system_clock::now();
  auto durationSinceEpoch = systemClockNow.time_since_epoch();
  unsigned long long currentTimeInNanos = durationSinceEpoch.count();
  std::cout << "Commit time in nanos: " << currentTimeInNanos << std::endl;
  tempCommitFile << "time\t" << currentTimeInNanos << std::endl;

  std::string word;
  //for now there are only 3 words in each line of index file: exec rights, hash and localization, to update later
  int amountOfWordsInLine = 3;
  int currentWordCount = 3;
  while (file >> word) {
    // displaying content
    if (word == "file" && currentWordCount % amountOfWordsInLine == 0) continue;
    if (currentWordCount % amountOfWordsInLine == 0) {
      tempCommitFile << "file\t";
      //Exec rights word (e for executable, n for not executable)
      std::cout << "Exec rights: " << word;
      tempCommitFile << word << " ";
      currentWordCount = 0;
    } else if (currentWordCount % amountOfWordsInLine == 1) {
      std::cout << " Hash: " << word;
      tempCommitFile << word << " ";
    } else if (currentWordCount % amountOfWordsInLine == 2) {
      std::cout << " File path: " << word;
      tempCommitFile << word << " ";
      std::cout << std::endl;
      tempCommitFile << std::endl;
    } else {
      std::cout << " Something different: " << word;
      tempCommitFile << "not supported!: " << word;
    }

    currentWordCount++;
  }

  if (std::filesystem::is_empty(MAIN_BRANCH_LOCALIZATION)) {
    std::cout << "There is no commit in main branch, setting current commit to HEAD" << std::endl;
  } else {
    std::cout << "Changing main branch HEAD to this commit" << std::endl;
  }
  std::fstream mainBranchFile(MAIN_BRANCH_LOCALIZATION, std::ios::out);
  if (!mainBranchFile.is_open()) {
    throw std::runtime_error("Failed to open the file: " + MAIN_BRANCH_LOCALIZATION);
  }
  std::string calculatedHash = calculateHash(TEMP_COMMIT_FILE_LOCALIZATION);
  mainBranchFile << calculatedHash << std::endl;

  std::filesystem::copy_file(TEMP_COMMIT_FILE_LOCALIZATION,
                             OBJECTS_FOLDER_LOCALIZATION + "/" + calculatedHash);

  std::filesystem::remove(TEMP_COMMIT_FILE_LOCALIZATION);
}

void MyGitErase() {
  if (std::filesystem::exists(MAIN_FOLDER_NAME)) {
    std::filesystem::remove_all(MAIN_FOLDER_NAME);
    std::cout << "Erased " << MAIN_FOLDER_NAME << " and all it's children" << std::endl;
  } else {
    std::cout << "File " << MAIN_FOLDER_NAME << " did not exist" << std::endl;
  }
}

void MyGitStatus() {
  std::ifstream indexFile(INDEX_FILE_LOCALIZATION, std::ios::binary);
  if (!indexFile.is_open()) {
    std::cout << "index file doesn't exist!" << std::endl;
    std::cout << "Maybe you forgot to './MyGit init' or './MyGit add'" << std::endl;
    return;
  }
  if (std::filesystem::exists(MAIN_BRANCH_LOCALIZATION)) {
    std::cout << "There is no commit in branch main, checking working directory vs index diff" << std::endl;
  } else {
    std::cout << "Checking HEAD vs index diff" << std::endl;
  }

  std::vector<std::pair<std::string, std::string> > filesFromIndex;
  filesFromIndex = getMyGitFiles(indexFile);

  for (auto const &[filePath, fileHash]: filesFromIndex) {
    std::cout << "index:" << filePath << std::endl;
  }
  std::ifstream headFileHashFile(MAIN_BRANCH_LOCALIZATION, std::ios::binary);
  if (!headFileHashFile.is_open()) {
    return;
  }

  std::string headFileHash;
  getline(headFileHashFile, headFileHash);
  std::cout << OBJECTS_FOLDER_LOCALIZATION + "/" + headFileHash << std::endl;
  std::ifstream headFile(OBJECTS_FOLDER_LOCALIZATION + "/" + headFileHash, std::ios::binary);
  if (!headFile.is_open()) {
    return;
  }

  std::vector<std::pair<std::string, std::string> > filesFromHead;
  filesFromHead = getMyGitFiles(headFile);

  for (auto const &[filePath, fileHash]: filesFromHead) {
    std::cout << "head:" << filePath << std::endl;
  }

  //TODO: delete this ugly n^2 loop for hashmap or something better
  for (auto const &[filePathHead, fileHashHead]: filesFromHead) {
    for (auto const &[filePathIndex, fileHashIndex]: filesFromIndex) {
      if (filePathHead == filePathIndex) {
        if (fileHashHead == fileHashIndex) {
          std::cout << filePathHead << " is the same in index and head" << std::endl;
        } else {
          std::cout << "modified: " << filePathHead << std::endl;
        }
      } else if (fileHashHead == fileHashIndex) {
        if (filePathHead == filePathIndex) {
          std::cout << filePathHead << " is the same in index and head" << std::endl;
        } else {
          std::cout << "name changed: " << filePathHead << std::endl;
        }
      }
    }
  }

  for (auto const &[filePath, fileHash]: filesFromIndex) {
    if (calculateHash(filePath) != fileHash) {
      std::cout << "Changes not staged for commit: " << filePath << std::endl;
      std::cout << calculateHash(filePath) << std::endl;
      std::cout << fileHash << std::endl;
    }
  }
}

std::vector<std::pair<std::string, std::string> > getMyGitFiles(std::ifstream &file) {
  std::string word;
  int amountOfWordsInLine = 4;
  int currentWordCount = 0;
  std::vector<std::pair<std::string, std::string> > filesToCheck;
  int currentIndex = 0;
  std::string fileHash;
  std::string filePath;
  std::string line;
  while (getline(file, line)) {
    // displaying content
    std::stringstream ss(line);
    std::string word;
    std::vector<std::string> result;

    // Extract words one by one
    while (ss >> word) {
      result.push_back(word);
    }
    if (result[0] != "file") continue;
    std::cout << " File hash: " << result[2] << std::endl;
    fileHash = result[2];

    std::cout << " File path: " << result[3] << std::endl;
    filePath = result[3];

    filesToCheck.push_back(std::make_pair(filePath, fileHash));

    currentWordCount++;
    if (amountOfWordsInLine < currentWordCount) {
      currentWordCount = 0;
    }
  }

  return filesToCheck;
}
