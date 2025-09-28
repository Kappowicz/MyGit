#include "mygit.h"
#include <string>
#include <iostream>

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
    MyGitAdd(second);
  } else if (first == "commit") {
    MyGitCommit();
  }

  return 0;
}
