#include "mygit.h"
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::println("start with 'mygit.exe --help'");
    printHelp();
    return 0;
  }

  std::string first = argv[1];
  if (first == "--help") {
    printHelp();
  } else if (first == "init") {
    MyGitInit();
  } else if (first == "add") {
    if (argc < 3) throw std::invalid_argument("Not enough arguments");
    std::string second = argv[2];
    MyGitAdd(second);
  } else if (first == "commit") {
    if (argc < 3) throw std::invalid_argument("Not enough arguments");
    std::string second = argv[2];
    if (second == "-m") {
      //for now there is only -m parameter
      std::string message = argv[3];
      if (message.empty()) {
        std::println("Commit message can't be empty!");
      } else {
        MyGitCommit(message);
      }
    } else {
      std::println("Not implemented mygit commit parameter: {}", second);
      std::println("Did you mean parameter '-m' (message)?");
    }
  } else if (first == "_erase") {
    //debug purpose only, doesn't exist in real git so starts with '_'
    //makes it easier to test because I don't need to remember where
    //the project base directory (with .git folder) is
    MyGitErase();
  } else if (first == "status") {
    MyGitStatus();
  } else if (first == "log")
    MyGitLog();
  else if (first == "checkout") {
    if (argc < 3) throw std::invalid_argument("Not enough arguments");
    std::string second = argv[2];
    MyGitCheckout(second);
  } else if (first == "hash-object") {
    if (argc < 3) throw std::invalid_argument("Not enough arguments");
    std::string second = argv[2];
    std::string output = MyGitHashObject(second);
    if (!output.empty()) {
      std::println("{}", output);
    }
  } else if (first == "diff") {
    MyGitDiff();
  } else if (first == "branch") {
    if (argc < 3) {
      MyGitBranch();
    } else {
      std::string second = argv[2];
      MyGitBranch(second);
    }
  } else if (first == "switch") {
    if (argc < 3) throw std::invalid_argument("Not enough arguments");
    std::string second = argv[2];
    MyGitSwitch(second);
  } else {
    std::println("Command {} not found", first);
    if (first == "erase") std::println("Did you mean '_erase'?");
  }

  return 0;
}
