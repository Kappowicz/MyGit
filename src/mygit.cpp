#include <iostream> // used for std::cout in LOG(x)
#include "mygit.h"
#include <filesystem> //available from c++17
#include <fstream>
#include <string>
#include <print>
#include <sstream>
#include <format> //for hex conversion

//colors defined to make output text colorful (used in './MyGit diff')
#define RED     "\033[31m"
#define GREEN   "\033[32m"

const std::string MAIN_FOLDER_NAME = ".mygit";
const std::string OBJECTS_FOLDER_NAME = "/objects";
const std::string REFS_FOLDER_NAME = "/refs";
const std::string INDEX_FILE_NAME = "/index";
const std::string TEMP_COMMIT_FILE_NAME = "/.tempcommit";
const std::string EXEC_CHAR = "e";
const std::string NOT_EXEC_CHAR = "n";
const std::string MAIN_BRANCH_NAME = "main";
const std::string HEAD_NAME = "HEAD";

const std::string INDEX_FILE_LOCALIZATION = MAIN_FOLDER_NAME + INDEX_FILE_NAME;
const std::string OBJECTS_FOLDER_LOCALIZATION =
    MAIN_FOLDER_NAME + OBJECTS_FOLDER_NAME;
const std::string TEMP_COMMIT_FILE_LOCALIZATION =
    MAIN_FOLDER_NAME + TEMP_COMMIT_FILE_NAME;

const std::string REFS_FOLDER_LOCALIZATION =
    MAIN_FOLDER_NAME + REFS_FOLDER_NAME;
const std::string MAIN_BRANCH_LOCALIZATION =
    REFS_FOLDER_LOCALIZATION + "/" + MAIN_BRANCH_NAME;
const std::string HEAD_LOCALIZATION =
    REFS_FOLDER_LOCALIZATION + "/" + HEAD_NAME;

#if defined(_DEBUG) || !defined(NDEBUG)
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x)
#endif

struct FileProperties {
  char execChar;
  std::string fileHash;
  std::string filePath;
};

void printHelp() {
  //inspired by real git --help
  std::println("{}", R"(this is my simple implementation of git
start by initializing the repo with './MyGit init'
Available commands:
start or delete a working area:
- 'mygit init' initialize mygit folder structure or reinitialize existing one
- 'mygit _erase' delete entire mygit repository (debug)
work on the current change:
- 'mygit add {fileName}' add file to index
examine the history and state:
- 'mygit log' show commits history
- 'mygit status' display status of files in current directory
- 'mygit diff' show differences in file content
- 'mygit hash-object {fileName}' display hash of a file
grow and tweak your history:
- 'mygit branch' display all branches
- 'mygit branch {branchName}' create branch
- 'mygit commit -m {commitName}' create commit
- 'mygit checkout {commitName}' (doesn't support switching branches yet) change current commit)");
}

void MyGitInit() {
  initDirectory(MAIN_FOLDER_NAME);
  initDirectory(OBJECTS_FOLDER_LOCALIZATION);
  initDirectory(REFS_FOLDER_LOCALIZATION);

  initFile(MAIN_BRANCH_LOCALIZATION);
  initFile(HEAD_LOCALIZATION);
}

void initDirectory(const std::string &directory) {
  if (std::filesystem::is_directory(directory)) {
    std::println("folder {} exists", directory);
  } else {
    if (std::filesystem::create_directories(directory)) {
      std::println("folder {} created", directory);
    } else {
      std::println("folder {} failed to create", directory);
    }
  }
}

void initFile(const std::string &fileName) {
  if (std::filesystem::exists(fileName)) {
    std::println("file {} exists", fileName);
    return;
  }
  if (std::ofstream file(fileName); !file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + fileName);
  }
  std::println("file {} created", fileName);
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

  constexpr int minHashSize = 15;
  std::string hexCount = std::format("{:0{}x}", count, minHashSize);
  LOG("Hash for " << fileName << ": " << hexCount);

  return hexCount;
}

void MyGitAdd(const std::string &fileName) {
  if (!std::filesystem::exists(MAIN_FOLDER_NAME)) {
    std::println("Folder {} doesn't exist!", MAIN_FOLDER_NAME);
    std::println("Maybe you didn't './MyGit init'?");
    return;
  }
  const std::string hash = calculateHash(fileName);
  const std::string fileDestination = OBJECTS_FOLDER_LOCALIZATION + "/" + hash;
  if (std::filesystem::exists(fileDestination)) {
    std::println("File {} already exists", fileDestination);
    return;
  }
  std::filesystem::copy_file(fileName, fileDestination);

  addToIndex(fileName, hash);
}

void addToIndex(const std::string &fileName, const std::string &hash) {
  std::ofstream file(INDEX_FILE_LOCALIZATION, std::ios::app);
  if (!file.is_open()) {
    throw std::runtime_error(
      "Failed to open the file2: " + INDEX_FILE_LOCALIZATION);
  }
  std::ifstream file2(INDEX_FILE_LOCALIZATION);
  if (!file2.is_open()) {
    std::println("Index file doesn't exist!");
    return;
  }

  std::vector<FileProperties> fileProperties = getMyGitFiles(file2);
  bool fileFound = false;
  for (auto const &[execChar, fileHash, filePath]: fileProperties) {
    if (filePath == fileName) {
      std::println("File name: {} already exists. ", filePath);
      fileFound = true;
    }
  }

  std::ofstream tempFile(INDEX_FILE_LOCALIZATION + ".temp");
  if (!tempFile.is_open()) {
    throw std::runtime_error(
      "Failed to open the file2: " + INDEX_FILE_LOCALIZATION);
  }

  if (fileFound) {
    std::string output;
    //change only the hash of the file
    for (auto const &[execChar, fileHash, filePath]: fileProperties) {
      if (filePath == fileName) {
        std::println("Changing only hash of the file in index");
        output = "file " + std::string{execChar} + " " + calculateHash(filePath)
                 + " " + filePath + "\n";
        tempFile.write(output.c_str(), output.size());
      } else {
        output = "file " + std::string{execChar} + " " + fileHash + " " +
                 filePath + "\n";
        tempFile.write(output.c_str(), output.size());
      }
    }
    tempFile.close();
    std::filesystem::copy_file(INDEX_FILE_LOCALIZATION + ".temp",
                               INDEX_FILE_LOCALIZATION,
                               std::filesystem::copy_options::overwrite_existing);
  } else {
    //write at the end of the file
    std::filesystem::path filePath(fileName);
    std::filesystem::file_status status = std::filesystem::status(filePath);
    std::filesystem::perms permissions = status.permissions();
    std::string currExecChar =
        (permissions & std::filesystem::perms::owner_exec) !=
        std::filesystem::perms::none
          ? EXEC_CHAR
          : NOT_EXEC_CHAR;
    std::string output = "file " + currExecChar + " " + hash + " " + fileName +
                         "\n";
    file.write(output.c_str(), output.size());
  }

  if (std::filesystem::remove(INDEX_FILE_LOCALIZATION + ".temp")) {
    LOG(INDEX_FILE_LOCALIZATION + ".temp" + " deleted");
  }
}

void MyGitCommit(const std::string &message) {
  /* sample commit object
tree f314a9254316e1a92a54466b81bdd09415c44136   <- this commit tree hash from index file
parent                                          <- previous commit hash from index file (empty for first commit)
Initial commit with siema.txt and ok.png        <- this commit's message
   */
  if (!std::filesystem::exists(INDEX_FILE_LOCALIZATION)) {
    std::println("File {} doesn't exist!", INDEX_FILE_LOCALIZATION);
    std::println(
      "Maybe you didn't './MyGit add' any files after './MyGit init'?");
    return;
  }
  std::ifstream file(INDEX_FILE_LOCALIZATION, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error(
      "Failed to open the file: " + INDEX_FILE_LOCALIZATION);
  }
  std::ofstream tempCommitFile(TEMP_COMMIT_FILE_LOCALIZATION, std::ios::binary);
  if (!tempCommitFile.is_open()) {
    throw std::runtime_error(
      "Failed to open the file: " + TEMP_COMMIT_FILE_LOCALIZATION);
  }

  LOG("new commit's message: " << message);
  tempCommitFile << "message\t" << message << std::endl;

  std::ifstream headFileHashFile(MAIN_BRANCH_LOCALIZATION, std::ios::binary);
  if (!headFileHashFile.is_open()) {
    return;
  }
  std::string headFileHash;
  getline(headFileHashFile, headFileHash);
  LOG(OBJECTS_FOLDER_LOCALIZATION + "/" + headFileHash);
  std::ifstream headFile(OBJECTS_FOLDER_LOCALIZATION + "/" + headFileHash,
                         std::ios::binary);
  if (!headFile.is_open()) {
    return;
  }

  if (headFileHash.empty()) {
    std::println("Parent Commit doesn't exist, so it's first commit");
  } else {
    std::println("Parent Commit: {}", headFileHash);
  }

  tempCommitFile << "parent\t" << headFileHash << std::endl;

  //get current time in nanoseconds
  auto systemClockNow = std::chrono::system_clock::now();
  auto durationSinceEpoch = systemClockNow.time_since_epoch();
  unsigned long long currentTimeInNanos = durationSinceEpoch.count();
  LOG("Commit time in nanos: " << currentTimeInNanos);
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
      std::print("Exec rights: {}", word);
      tempCommitFile << word << " ";
      currentWordCount = 0;
    } else if (currentWordCount % amountOfWordsInLine == 1) {
      std::print(" Hash: {}", word);
      tempCommitFile << word << " ";
    } else if (currentWordCount % amountOfWordsInLine == 2) {
      std::println(" File path: {}", word);
      tempCommitFile << word << " " << std::endl;
    } else {
      std::print(" Something different: {}", word);
      tempCommitFile << "not supported!: " << word;
    }

    currentWordCount++;
  }

  if (std::filesystem::is_empty(MAIN_BRANCH_LOCALIZATION)) {
    std::println(
      "There is no commit in main branch, setting current commit to HEAD");
  } else {
    std::println("Changing main branch HEAD to this commit");
  }
  std::fstream mainBranchFile(MAIN_BRANCH_LOCALIZATION, std::ios::out);
  if (!mainBranchFile.is_open()) {
    throw std::runtime_error(
      "Failed to open the file: " + MAIN_BRANCH_LOCALIZATION);
  }
  std::string calculatedHash = calculateHash(TEMP_COMMIT_FILE_LOCALIZATION);
  mainBranchFile << calculatedHash << std::endl;
  writeToHead(calculatedHash);
  std::filesystem::copy_file(TEMP_COMMIT_FILE_LOCALIZATION,
                             OBJECTS_FOLDER_LOCALIZATION + "/" +
                             calculatedHash);

  std::filesystem::remove(TEMP_COMMIT_FILE_LOCALIZATION);
}

void MyGitErase() {
  if (std::filesystem::exists(MAIN_FOLDER_NAME)) {
    std::filesystem::remove_all(MAIN_FOLDER_NAME);
    std::println("Erased {} and all it's children", MAIN_FOLDER_NAME);
  } else {
    std::println("File {} did not exist", MAIN_FOLDER_NAME);
  }
}

void MyGitStatus() {
  std::ifstream indexFile(INDEX_FILE_LOCALIZATION, std::ios::binary);
  if (!indexFile.is_open()) {
    std::println("Index file doesn't exist!");
    std::println("Maybe you forgot to './MyGit init' or './MyGit add'");
    return;
  }

  std::vector<FileProperties> filesFromIndex = getMyGitFiles(indexFile);

  for (auto const &[execChar, fileHash, filePath]: filesFromIndex) {
    LOG("index:" << filePath);
  }

  if (std::filesystem::is_empty(MAIN_BRANCH_LOCALIZATION)) {
    std::println(
      "There is no commit in branch main, checking working directory vs index diff");
  } else {
    std::println("Checking HEAD vs index diff");
    compareHeadAndIndex(filesFromIndex);
  }

  for (auto const &[execChar, fileHash, filePath]: filesFromIndex) {
    if (calculateHash(filePath) != fileHash) {
      std::println("Changes not staged for commit: {}", filePath);
      std::println("{}", calculateHash(filePath));
      std::println("{}", fileHash);
    }
  }
}

void compareHeadAndIndex(const std::vector<FileProperties> &filesFromIndex) {
  std::ifstream headFileHashFile(MAIN_BRANCH_LOCALIZATION, std::ios::binary);
  if (!headFileHashFile.is_open()) {
    return;
  }
  std::string headFileHash;
  getline(headFileHashFile, headFileHash);
  if (headFileHash.empty())
    throw std::runtime_error(
      "Empty hash file even if in previous function it wasn't");
  LOG(OBJECTS_FOLDER_LOCALIZATION + "/" + headFileHash);
  std::ifstream headFile(OBJECTS_FOLDER_LOCALIZATION + "/" + headFileHash,
                         std::ios::binary);
  if (!headFile.is_open()) {
    return;
  }

  std::vector<FileProperties> filesFromHead;
  filesFromHead = getMyGitFiles(headFile);

  for (auto const &[execChar, fileHash, filePath]: filesFromHead) {
    LOG("head:" << filePath);
  }
  //TODO: make it show changes made in the front of the file correctly
  //not like now it is showing as if all lines were changed
  //TODO: delete this ugly n^2 loop for hashmap or something better
  for (auto const &[execCharHead, fileHashHead, filePathHead]: filesFromHead) {
    for (auto const &[execCharIndex, fileHashIndex, filePathIndex]:
         filesFromIndex) {
      if (filePathHead == filePathIndex) {
        if (fileHashHead == fileHashIndex) {
          std::println("{} is the same in index and head", filePathHead);
        } else {
          std::println("modified: {}", filePathHead);
        }
      } else if (fileHashHead == fileHashIndex) {
        if (filePathHead == filePathIndex) {
          std::println("{} is the same in index and head", filePathHead);
        } else {
          std::println("name changed: {}", filePathHead);
        }
      }
    }
  }
}

std::vector<FileProperties> getMyGitFiles(std::ifstream &file) {
  constexpr int amountOfWordsInLine = 4;
  int currentWordCount = 0;

  std::vector<FileProperties> fileProperties;
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

    char fileExecChar = result[1][0];
    std::string fileHash = result[2];
    std::string filePath = result[3];

    fileProperties.push_back(
      {fileExecChar, fileHash, filePath}
    );
    LOG(" File exec: " << fileExecChar);
    LOG(" File hash: " << result[2]);
    LOG(" File path: " << result[3]);

    currentWordCount++;
    if (amountOfWordsInLine < currentWordCount) {
      currentWordCount = 0;
    }
  }

  return fileProperties;
}

void MyGitLog() {
  std::ifstream headFileHashFile(MAIN_BRANCH_LOCALIZATION, std::ios::binary);
  if (!headFileHashFile.is_open()) {
    return;
  }
  std::string headFileHash;
  getline(headFileHashFile, headFileHash);
  std::println("{}/{}", OBJECTS_FOLDER_LOCALIZATION, headFileHash);
  std::ifstream headFile(OBJECTS_FOLDER_LOCALIZATION + "/" + headFileHash,
                         std::ios::binary);
  if (!headFile.is_open()) {
    return;
  }

  std::string nextCommitToFind = headFileHash;
  while (!nextCommitToFind.empty()) {
    std::ifstream nextCommit(
      OBJECTS_FOLDER_LOCALIZATION + "/" + nextCommitToFind, std::ios::binary);
    if (!headFile.is_open()) {
      return;
    }
    std::string commitMessage;
    getline(nextCommit, commitMessage);
    std::println("{} {}", commitMessage, nextCommitToFind);

    std::string line;
    getline(nextCommit, line);

    std::stringstream ss(line);
    std::string word;
    std::vector<std::string> result;

    // Extract words one by one
    while (ss >> word) {
      result.push_back(word);
    }

    nextCommitToFind = result[1];

    getline(nextCommit, line);

    while (getline(nextCommit, line)) {
      result.erase(result.begin(), result.end());
      //print edited files
      std::stringstream ss2(line);

      // Extract words one by one
      while (ss2 >> word) {
        result.push_back(word);
      }

      std::println("{}", result[3]);
    }
  }
}

void MyGitCheckout(const std::string &commitName, bool isSwitchingBranch) {
  LOG("checkout " << commitName);

  std::ifstream commitFile(OBJECTS_FOLDER_LOCALIZATION + "/" + commitName,
                           std::ios::binary);
  if (!commitFile.is_open()) {
    std::println("commit not found");
    return;
  }

  writeToHead(commitName);

  std::ifstream mainBranchFile(MAIN_BRANCH_LOCALIZATION, std::ios::binary);
  if (!mainBranchFile.is_open()) {
    return;
  }
  std::string mainBranchCommitHash;
  getline(mainBranchFile, mainBranchCommitHash);

  if (commitName != mainBranchCommitHash && !isSwitchingBranch) {
    std::println("HEAD is now detached from any branch."
      "ANY OPERATIONS ON DETACHED HEAD are not supported yet");
  }

  std::string commitFileMessage;
  getline(commitFile, commitFileMessage);
  LOG(commitFileMessage);

  std::vector<FileProperties> fileProperties = getMyGitFiles(commitFile);
  for (auto const &file: fileProperties) {
    LOG(file.filePath);
    std::filesystem::copy_file(
      OBJECTS_FOLDER_LOCALIZATION + "/" + file.fileHash, file.filePath,
      std::filesystem::copy_options::overwrite_existing);
    LOG("copied file from; " << OBJECTS_FOLDER_LOCALIZATION + "/" + file.
      fileHash << " to: " + file.filePath);
  }
}

void writeToHead(const std::string &message) {
  std::ofstream headFile(HEAD_LOCALIZATION, std::ios::trunc);
  if (!headFile.is_open()) {
    std::println("HEAD file not found");
    return;
  }
  headFile << message << std::endl;
}

std::string getFromHead() {
  std::ifstream headFile(HEAD_LOCALIZATION);
  if (!headFile.is_open()) {
    throw std::runtime_error("Failed to open the file: " + HEAD_LOCALIZATION);
  }
  std::string output;
  getline(headFile, output);
  return output;
}

std::string MyGitHashObject(const std::string &filename) {
  if (std::ifstream fileToCheck(filename); !fileToCheck.is_open()) {
    std::println("File {} doesn't exist!", filename);
    return {};
  }
  return calculateHash(filename);
}

void MyGitDiff() {
  std::ifstream indexFile(INDEX_FILE_LOCALIZATION, std::ios::binary);
  if (!indexFile.is_open()) {
    std::println("Index file doesn't exist!");
    std::println("Maybe you forgot to './MyGit init' or './MyGit add'");
    return;
  }

  std::vector<FileProperties> filesFromIndex = getMyGitFiles(indexFile);

  for (auto const &[execChar, fileHash, filePath]: filesFromIndex) {
    if (calculateHash(filePath) != fileHash) {
      std::println("Changes not staged for commit: {}", filePath);

      std::ifstream file(filePath);
      if (!file.is_open()) {
        std::println("File {} doesn't exist!", filePath);
        return;
      }

      std::ifstream fileFromIndex(OBJECTS_FOLDER_LOCALIZATION + "/" + fileHash,
                                  std::ios::binary);
      if (!fileFromIndex.is_open()) {
        std::println("file {}/{} doesn't exist!", OBJECTS_FOLDER_LOCALIZATION,
                     fileHash);
        return;
      }
      std::string indexLine;
      std::string fileLine;
      bool isLine = false;
      do {
        isLine = false;
        getline(fileFromIndex, indexLine);
        if (!indexLine.empty()) isLine = true;
        getline(file, fileLine);
        if (!fileLine.empty()) isLine = true;
        if (indexLine != fileLine) {
          if (!indexLine.empty()) {
            std::println("{}-  {}", RED, indexLine);
          }
          if (!fileLine.empty()) {
            std::println("{}+  {}", GREEN, fileLine);
          }
        }
      } while (isLine);
    }
  }
}

void MyGitBranch() {
  for (auto file:
       std::filesystem::directory_iterator(REFS_FOLDER_LOCALIZATION)) {
    if (file.path().filename() == HEAD_NAME) continue;

    std::ifstream tempFile(file.path());
    if (!tempFile.is_open()) {
      throw std::runtime_error(
        "Failed to open the file: " + INDEX_FILE_LOCALIZATION);
    }
    std::string branchCommitHash;
    getline(tempFile, branchCommitHash);
    std::println("{} {}", file.path().filename().string(), branchCommitHash);
  }
}

void MyGitBranch(const std::string &branchName) {
  if (std::ofstream headFile(REFS_FOLDER_LOCALIZATION + "/" + branchName);
    !headFile.is_open()) {
    throw std::runtime_error(
      "Failed to open the file: " + REFS_FOLDER_LOCALIZATION + "/" +
      branchName);
  }
  std::println("branch {} created", branchName);
}

void MyGitSwitch(const std::string &branchName) {
  std::println("{}", getFromHead());
  std::ifstream newBranchFile(REFS_FOLDER_LOCALIZATION + "/" + branchName,
                              std::ios::binary);
  if (!newBranchFile.is_open()) {
    std::println("Branch {}/{} doesn't exist!", REFS_FOLDER_LOCALIZATION,
                 branchName);
    return;
  }
  std::string newBranchHashLine;
  getline(newBranchFile, newBranchHashLine);
  writeToHead(newBranchHashLine);
  std::println("new branch commit: {}", newBranchHashLine);

  MyGitCheckout(newBranchHashLine, true);
}
