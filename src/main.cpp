#include "mygit.h"
#include <string>
#include <filesystem>
#include <iostream>

const std::string MAIN_FOLDER_NAME = ".mygit";

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << "start with 'mygit.exe --help'" << std::endl;
    printHelp();
    return 0;
  }

  std::string first = argv[1];
  if (first == "--help") {
    printHelp();
  } else if (first == "init") {
    initMyGit();
  } else if (first == "add") {
    std::string second = argv[2];
    std::string result = calculateHash(second);
  }

  return 0;
}
