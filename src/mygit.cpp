#include <iostream> // used for std::cout in LOG(x) macro
#include "mygit.h"
#include <filesystem> //available from c++17
#include <fstream>
#include <string>
#include <print>
#include <sstream>
#include <format> //for hex conversion
#include <map>
#include <ranges>

//colors defined to make output text colorful (used in './MyGit diff')
#define RED     "\033[31m"
#define GREEN   "\033[32m"

//using string_view for optimization
constexpr std::string_view MAIN_FOLDER_NAME = ".mygit";
constexpr std::string_view OBJECTS_FOLDER_NAME = "objects";
constexpr std::string_view REFS_FOLDER_NAME = "refs";
constexpr std::string_view INDEX_FILE_NAME = "index";
constexpr std::string_view TEMP_COMMIT_FILE_NAME = ".tempcommit";
constexpr std::string_view MAIN_BRANCH_NAME = "main";
constexpr std::string_view HEAD_NAME = "HEAD";

constexpr std::string EXEC_CHAR = "e";
constexpr std::string NOT_EXEC_CHAR = "n";

//using path instead of string to make it os-agnostic
const std::filesystem::path MAIN_FOLDER_PATH = MAIN_FOLDER_NAME;
const std::filesystem::path INDEX_FILE_PATH = MAIN_FOLDER_PATH / INDEX_FILE_NAME;
const std::filesystem::path OBJECTS_FOLDER_PATH = MAIN_FOLDER_PATH / OBJECTS_FOLDER_NAME;
const std::filesystem::path TEMP_COMMIT_FILE_PATH = MAIN_FOLDER_PATH / TEMP_COMMIT_FILE_NAME;

const std::filesystem::path REFS_FOLDER_PATH = MAIN_FOLDER_PATH / REFS_FOLDER_NAME;

const std::filesystem::path MAIN_BRANCH_PATH = REFS_FOLDER_PATH / MAIN_BRANCH_NAME;
const std::filesystem::path HEAD_PATH = REFS_FOLDER_PATH / HEAD_NAME;

#if defined(_DEBUG) || !defined(NDEBUG)
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x)
#endif

struct FileProperties {
  char execChar;
  std::string fileHash;
  std::filesystem::path filePath;
};

std::vector<std::string_view> getCloseStrings(const std::string_view &target,
                                              const std::vector<std::string_view> &available) {
  constexpr int allowedDiffRange = 2;
  std::vector<std::string_view> output;
  std::map<char, int> targetStringCharFrequency;

  for (char c: target) {
    targetStringCharFrequency[c]++;
  }
  std::map<char, int> tempFrequency(targetStringCharFrequency);

  std::map<char, int> currentStringCharFrequency;
  for (std::string_view currentCheckedCommand: available) {
    targetStringCharFrequency = tempFrequency;
    currentStringCharFrequency.clear();
    for (char currentCheckedChar: currentCheckedCommand) {
      currentStringCharFrequency[currentCheckedChar]++;
    }

    int diff = 0;
    for (auto &[key, val]: targetStringCharFrequency) {
      diff += abs(currentStringCharFrequency[key] - targetStringCharFrequency[key]);
      currentStringCharFrequency[key] = 0;
      targetStringCharFrequency[key] = 0;
    }
    for (auto &[key, val]: currentStringCharFrequency) {
      diff += abs(currentStringCharFrequency[key] - targetStringCharFrequency[key]);
      currentStringCharFrequency[key] = 0;
      targetStringCharFrequency[key] = 0;
    }

    if (diff <= allowedDiffRange) {
      output.push_back(currentCheckedCommand);
    }
  }

  return output;
}

void MyGitHelp() {
  //inspired by real git --help
  std::println("{}", R"(this is my simple implementation of git
start by initializing the repo with './MyGit init'
Available commands:
start or delete a working area:
- 'mygit init' initialize mygit folder structure or reinitialize existing one
- 'mygit _erase' delete entire mygit repository (debug)
work on the current change:
- 'mygit add <fileName>' add file to index
examine the history and state:
- 'mygit log' show commits history
- 'mygit status' display status of files in current directory
- 'mygit diff' show differences in file content
- 'mygit hash-object <fileName>' display hash of a file
grow and tweak your history:
- 'mygit branch' display all branches
- 'mygit branch <branchName>' create branch
- 'mygit commit -m <commitName>' create commit
- 'mygit checkout <commitName>' (doesn't support switching branches yet) change current commit)");
}

void MyGitInit() {
  initDirectory(MAIN_FOLDER_PATH);
  initDirectory(OBJECTS_FOLDER_PATH);
  initDirectory(REFS_FOLDER_PATH);

  initFile(MAIN_BRANCH_PATH);
  initFile(HEAD_PATH);
}

void initDirectory(const std::filesystem::path &directoryPath) {
  if (std::filesystem::is_directory(directoryPath)) {
    std::println("folder {} exists", directoryPath.string());
  } else {
    if (std::filesystem::create_directories(directoryPath)) {
      std::println("folder {} created", directoryPath.string());
    } else {
      std::println("folder {} failed to create", directoryPath.string());
    }
  }
}

void initFile(const std::filesystem::path &filePath) {
  if (std::filesystem::exists(filePath)) {
    std::println("file {} exists", filePath.string());
    return;
  }
  if (std::ofstream file(filePath); !file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + filePath.string());
  }
  std::println("file {} created", filePath.string());
}

//TODO: add unexpected
//something like djb2
std::string calculateHash(const std::filesystem::path &filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + filePath.string());
  }

  unsigned long long count = 1;
  char c;
  while (file.get(c)) {
    count = ((count << 2) + count) + c;
  }

  constexpr int minHashSize = 5;
  std::string hexCount = std::format("{:0{}x}", count, minHashSize);
  LOG("Hash for " << filePath << ": " << hexCount);

  return hexCount;
}

//TODO: convert exceptions to unexpected
void MyGitAdd(const std::vector<std::string_view> &arguments) {
  if (arguments.size() < 3 || arguments.size() > 4) {
    std::println(stderr, "Wrong number of arguments!");
    std::println(stderr, "Usage: ./mygit add <fileName>");
    return;
  }

  if (!std::filesystem::exists(MAIN_FOLDER_PATH)) {
    std::println(stderr, "Folder '{}' doesn't exist!", MAIN_FOLDER_PATH.string());
    std::println(stderr, "Maybe you didn't './MyGit init'?");
    return;
  }

  //TODO: delete all strings in my code (in this example convert to std::filesystem::path)
  const std::filesystem::path filePath(arguments[2]);
  if (!std::filesystem::exists(filePath)) {
    std::println(stderr, "File '{}' doesn't exist!", filePath.string());
    return;
  }

  const std::string hash = calculateHash(filePath);
  const std::filesystem::path fileDestination = OBJECTS_FOLDER_PATH / hash;
  if (std::filesystem::exists(fileDestination)) {
    std::println("File with hash '{}' is already present in index. Nothing to do.",
                 fileDestination.filename().string());
    return;
  }

  std::filesystem::copy_file(filePath, fileDestination);

  addToIndex(filePath, hash);
}

void addToIndex(const std::filesystem::path &filePathToAdd, const std::string &hash) {
  std::ofstream inputIndexFile(INDEX_FILE_PATH, std::ios::app);
  if (!inputIndexFile.is_open()) {
    throw std::runtime_error("Failed to open the file2: " + INDEX_FILE_PATH.string());
  }
  std::ifstream outputIndexFile(INDEX_FILE_PATH);
  if (!outputIndexFile.is_open()) {
    std::println(stderr, "Index file doesn't exist!");
    return;
  }

  std::vector<FileProperties> fileProperties = getMyGitFiles(outputIndexFile);
  bool fileFound = false;
  for (auto const &[_, _, filePathToCheck]: fileProperties) {
    if (filePathToCheck == filePathToAdd) {
      std::println("File name: {} already exists. ", filePathToAdd.string());
      fileFound = true;
      break;
    }
  }

  const std::filesystem::path tempFilePath = std::move(
    std::filesystem::path(INDEX_FILE_PATH).concat(".temp"));
  std::ofstream tempFile(tempFilePath);
  if (!tempFile.is_open()) {
    throw std::runtime_error("Failed to open the file2: " + INDEX_FILE_PATH.string());
  }

  //TODO: refactor this to use temp file and replace existing index
  //with temp with new values, it's safer and faster
  if (fileFound) {
    std::string output;
    //change only the hash of the file
    for (auto const &[execCharToCheck, fileHashToCheck, filePathToCheck]: fileProperties) {
      if (filePathToCheck == filePathToAdd) {
        std::println("Changing only hash of the file in index");
        output = "file " + std::string{execCharToCheck} + " " + calculateHash(filePathToAdd) + " " +
                 filePathToAdd.string() + "\n";
        tempFile.write(output.c_str(), output.size());
      } else {
        output = "file " + std::string{execCharToCheck} + " " + fileHashToCheck + " " +
                 filePathToAdd.string() + "\n";
        tempFile.write(output.c_str(), output.size());
      }
    }
    tempFile.close();
    std::filesystem::copy_file(tempFilePath, INDEX_FILE_PATH,
                               std::filesystem::copy_options::overwrite_existing);
  } else {
    //write at the end of the file
    std::filesystem::file_status status = std::filesystem::status(filePathToAdd);
    std::filesystem::perms permissions = status.permissions();
    std::string currExecChar = (permissions & std::filesystem::perms::owner_exec) !=
                               std::filesystem::perms::none
                                 ? EXEC_CHAR
                                 : NOT_EXEC_CHAR;
    std::string output = "file " + currExecChar + " " + hash + " " + filePathToAdd.string() + "\n";
    inputIndexFile.write(output.c_str(), output.size());
  }

  if (std::filesystem::remove(tempFilePath)) {
    LOG(INDEX_FILE_PATH.string() + ".temp" + " deleted");
  }
}

void MyGitCommit(const std::vector<std::string_view> &arguments) {
  /* sample commit object
tree f314a9254316e1a92a54466b81bdd09415c44136   <- this commit tree hash from index file
parent                                          <- previous commit hash from index file (empty for first commit)
Initial commit with siema.txt and ok.png        <- this commit's message
   */
  if (arguments.size() < 4 || arguments.size() > 5) {
    std::println(stderr, "Wrong number of arguments!");
    std::println(stderr, "Usage: ./mygit commit -m <message>");
    return;
  }

  if (!std::filesystem::exists(INDEX_FILE_PATH)) {
    std::println(stderr, "File {} doesn't exist!", INDEX_FILE_PATH.string());
    std::println("Maybe you didn't './MyGit add' any files after './MyGit init'?");
    return;
  }
  std::ifstream file(INDEX_FILE_PATH, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + INDEX_FILE_PATH.string());
  }
  std::ofstream tempCommitFile(TEMP_COMMIT_FILE_PATH, std::ios::binary);
  if (!tempCommitFile.is_open()) {
    throw std::runtime_error("Failed to open the file: " + TEMP_COMMIT_FILE_PATH.string());
  }

  std::string_view commitMessage = arguments[3];
  LOG("new commit's message: " << commitMessage);
  tempCommitFile << "message\t" << commitMessage << std::endl;

  std::ifstream headFileHashFile(MAIN_BRANCH_PATH, std::ios::binary);
  if (!headFileHashFile.is_open()) {
    return;
  }
  std::string headFileHash;
  getline(headFileHashFile, headFileHash);
  LOG(OBJECTS_FOLDER_PATH.string() + "/" + headFileHash);

  std::filesystem::path headFileHashPath = OBJECTS_FOLDER_PATH / headFileHash;
  std::ifstream headFile(headFileHashPath, std::ios::binary);
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

  if (std::filesystem::is_empty(MAIN_BRANCH_PATH)) {
    std::println("There is no commit in main branch, setting current commit to HEAD");
  } else {
    std::println("Changing main branch HEAD to this commit");
  }
  std::fstream mainBranchFile(MAIN_BRANCH_PATH, std::ios::out);
  if (!mainBranchFile.is_open()) {
    throw std::runtime_error("Failed to open the file: " + MAIN_BRANCH_PATH.string());
  }
  std::string calculatedHash = calculateHash(TEMP_COMMIT_FILE_PATH);
  mainBranchFile << calculatedHash << std::endl;
  writeToHead(calculatedHash);
  std::filesystem::path calculatedHashPath = OBJECTS_FOLDER_PATH / calculatedHash;
  std::filesystem::copy_file(TEMP_COMMIT_FILE_PATH, calculatedHashPath);

  std::filesystem::remove(TEMP_COMMIT_FILE_PATH);
}

void MyGitErase() {
  if (std::filesystem::exists(MAIN_FOLDER_PATH)) {
    std::filesystem::remove_all(MAIN_FOLDER_PATH);
    std::println("Erased {} and all it's children", MAIN_FOLDER_PATH.string());
  } else {
    std::println("File {} did not exist", MAIN_FOLDER_PATH.string());
  }
}

void MyGitStatus() {
  std::ifstream indexFile(INDEX_FILE_PATH, std::ios::binary);
  if (!indexFile.is_open()) {
    std::println(stderr, "Index file doesn't exist!");
    std::println(stderr, "Maybe you forgot to './MyGit init' or './MyGit add'");
    return;
  }

  std::vector<FileProperties> filesFromIndex = getMyGitFiles(indexFile);

  for (auto const &[_, _, filePath]: filesFromIndex) {
    LOG("index:" << filePath);
  }

  if (std::filesystem::is_empty(MAIN_BRANCH_PATH)) {
    std::println("There is no commit in branch main, checking working directory vs index diff");
  } else {
    std::println("Checking HEAD vs index diff");
    compareHeadAndIndex(filesFromIndex);
  }

  for (auto const &[_, fileHash, filePath]: filesFromIndex) {
    if (calculateHash(filePath) != fileHash) {
      std::println("Changes not staged for commit: {}", filePath.string());
      std::println("{}", calculateHash(filePath));
      std::println("{}", fileHash);
    }
  }
}

void compareHeadAndIndex(const std::vector<FileProperties> &filesFromIndex) {
  std::ifstream headFileHashFile(MAIN_BRANCH_PATH, std::ios::binary);
  if (!headFileHashFile.is_open()) {
    return;
  }
  std::string headFileHash;
  getline(headFileHashFile, headFileHash);
  if (headFileHash.empty()) throw std::runtime_error(
    "Empty hash file even if in previous function it wasn't");
  LOG(OBJECTS_FOLDER_PATH.string() + "/" + headFileHash);
  std::filesystem::path headFileHashPath = OBJECTS_FOLDER_PATH / headFileHash;
  std::ifstream headFile(headFileHashPath, std::ios::binary);
  if (!headFile.is_open()) {
    return;
  }

  std::vector<FileProperties> filesFromHead;
  filesFromHead = getMyGitFiles(headFile);

  for (auto const &[_, _, filePath]: filesFromHead) {
    LOG("head:" << filePath);
  }
  //TODO: make it show changes made in the front of the file correctly
  //not like now it is showing as if all lines were changed
  //TODO: delete this ugly n^2 loop for hashmap or something better
  for (auto const &[_, fileHashHead, filePathHead]: filesFromHead) {
    for (auto const &[_, fileHashIndex, filePathIndex]: filesFromIndex) {
      if (filePathHead == filePathIndex) {
        if (fileHashHead == fileHashIndex) {
          std::println("{} is the same in index and head", filePathHead.string());
        } else {
          std::println("modified: {}", filePathHead.string());
        }
      } else if (fileHashHead == fileHashIndex) {
        if (filePathHead == filePathIndex) {
          std::println("{} is the same in index and head", filePathHead.string());
        } else {
          std::println("name changed: {}", filePathHead.string());
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

    fileProperties.push_back({fileExecChar, fileHash, filePath});
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
  std::ifstream headFileHashFile(MAIN_BRANCH_PATH, std::ios::binary);
  if (!headFileHashFile.is_open()) {
    return;
  }
  std::string headFileHash;
  getline(headFileHashFile, headFileHash);
  std::println("{}/{}", OBJECTS_FOLDER_PATH.string(), headFileHash);
  std::filesystem::path headFileHashPath = OBJECTS_FOLDER_PATH / headFileHash;
  std::ifstream headFile(headFileHashPath, std::ios::binary);
  if (!headFile.is_open()) {
    return;
  }

  std::string nextCommitToFind = headFileHash;
  while (!nextCommitToFind.empty()) {
    std::filesystem::path nextCommitPath = OBJECTS_FOLDER_PATH / nextCommitToFind;
    std::ifstream nextCommit(nextCommitPath, std::ios::binary);
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
  std::filesystem::path commitPath = OBJECTS_FOLDER_PATH / commitName;
  std::ifstream commitFile(commitPath, std::ios::binary);
  if (!commitFile.is_open()) {
    std::println("commit not found");
    return;
  }

  writeToHead(commitName);

  std::ifstream mainBranchFile(MAIN_BRANCH_PATH, std::ios::binary);
  if (!mainBranchFile.is_open()) {
    return;
  }
  std::string mainBranchCommitHash;
  getline(mainBranchFile, mainBranchCommitHash);

  if (commitName != mainBranchCommitHash && !isSwitchingBranch) {
    std::println(
      "HEAD is now detached from any branch."
      "ANY OPERATIONS ON DETACHED HEAD are not supported yet");
  }

  std::string commitFileMessage;
  getline(commitFile, commitFileMessage);
  LOG(commitFileMessage);

  std::vector<FileProperties> fileProperties = getMyGitFiles(commitFile);
  for (auto const &file: fileProperties) {
    LOG(file.filePath);
    std::filesystem::path fileHashPath = OBJECTS_FOLDER_PATH / file.fileHash;
    std::filesystem::copy_file(fileHashPath, file.filePath,
                               std::filesystem::copy_options::overwrite_existing);
    LOG("copied file from; " << OBJECTS_FOLDER_PATH.string() + "/" + file. fileHash << " to: " +
      file.filePath.string());
  }
}

void writeToHead(const std::string &message) {
  std::ofstream headFile(HEAD_PATH, std::ios::trunc);
  if (!headFile.is_open()) {
    std::println("HEAD file not found");
    return;
  }
  headFile << message << std::endl;
}

std::string getFromHead() {
  std::ifstream headFile(HEAD_PATH);
  if (!headFile.is_open()) {
    throw std::runtime_error("Failed to open the file: " + HEAD_PATH.string());
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
  std::ifstream indexFile(INDEX_FILE_PATH, std::ios::binary);
  if (!indexFile.is_open()) {
    std::println("Index file doesn't exist!");
    std::println("Maybe you forgot to './MyGit init' or './MyGit add'");
    return;
  }

  std::vector<FileProperties> filesFromIndex = getMyGitFiles(indexFile);

  for (auto const &[execChar, fileHash, filePath]: filesFromIndex) {
    if (calculateHash(filePath) != fileHash) {
      std::println("Changes not staged for commit: {}", filePath.string());

      std::ifstream file(filePath);
      if (!file.is_open()) {
        std::println("File {} doesn't exist!", filePath.string());
        return;
      }
      std::filesystem::path fileHashPath = OBJECTS_FOLDER_PATH / fileHash;
      std::ifstream fileFromIndex(fileHashPath, std::ios::binary);
      if (!fileFromIndex.is_open()) {
        std::println("file {}/{} doesn't exist!", OBJECTS_FOLDER_PATH.string(), fileHash);
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
  for (auto file: std::filesystem::directory_iterator(REFS_FOLDER_PATH)) {
    if (file.path().filename() == HEAD_NAME) continue;

    std::ifstream tempFile(file.path());
    if (!tempFile.is_open()) {
      throw std::runtime_error("Failed to open the file: " + INDEX_FILE_PATH.string());
    }
    std::string branchCommitHash;
    getline(tempFile, branchCommitHash);
    std::println("{} {}", file.path().filename().string(), branchCommitHash);
  }
}

void MyGitBranch(const std::string &branchName) {
  std::filesystem::path branchPath = REFS_FOLDER_PATH / branchName;
  if (std::ofstream headFile(branchPath); !headFile.is_open()) {
    throw std::runtime_error(
      "Failed to open the file: " + REFS_FOLDER_PATH.string() + "/" + branchName);
  }
  std::println("branch {} created", branchName);
}

void MyGitSwitch(const std::string &branchName) {
  std::println("{}", getFromHead());
  std::filesystem::path branchPath = REFS_FOLDER_PATH / branchName;
  std::ifstream newBranchFile(branchPath, std::ios::binary);
  if (!newBranchFile.is_open()) {
    std::println("Branch {}/{} doesn't exist!", REFS_FOLDER_PATH.string(), branchName);
    return;
  }
  std::string newBranchHashLine;
  getline(newBranchFile, newBranchHashLine);
  writeToHead(newBranchHashLine);
  std::println("new branch commit: {}", newBranchHashLine);

  MyGitCheckout(newBranchHashLine, true);
}
