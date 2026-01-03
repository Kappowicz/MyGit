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
    MyGitInit();
  } else if (first == "add") {
    std::string second = argv[2];
    MyGitAdd(second);
  } else if (first == "commit") {
    std::string second = argv[2];
    if (second == "-m") {
      //for now there is only -m parameter
      std::string message = argv[3];
      if (message.empty()) {
        std::cout << "Commit message can't be empty!" << std::endl;
      } else {
        MyGitCommit(message);
      }
    } else {
      std::cout << "Not implemented mygit commit parameter: " << second << std::endl;
      std::cout << "Did you mean parameter '-m' (message)?" << std::endl;
    }
  } else if (first == "_erase") {
    //debug purpose only, doesn't exist in real git so started with _
    //makes it easier to test because I don't need to remember where
    //the project base directory (with .git folder) is
    MyGitErase();
  } else if (first == "status") {
    MyGitStatus();
  } else if (first == "log")
    MyGitLog();
  else if (first == "checkout") {
    std::string second = argv[2];
    MyGitCheckout(second);
  } else if (first == "hash-object") {
    std::string second = argv[2];
    std::string output = MyGitHashObject(second);
    if (!output.empty()) {
      std::cout << output << std::endl;
    }
  } else {
    std::cout << "Command '" << first << "' not found" << std::endl;
    if (first == "erase") std::cout << "Did you mean '_erase'?" << std::endl;
  }

  return 0;
}
