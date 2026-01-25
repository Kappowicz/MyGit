#include "mygit.h"
#include <string>
#include <print>
#include <map>
#include <functional>
#include <ranges>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::println("start with 'mygit.exe --help'");
    MyGitHelp();
    return 0;
  }

  //convert input chars to string_view for easier processing
  const std::vector<std::string_view> arguments(argv, argv + argc);

  //TODO: add aliases and 'did you mean?'
  //for small data sets its better to not use 'unordered' types
  std::map<std::string, std::function<void(std::vector<std::string_view>)> > commands = {
    {"--help", [](const auto &args) { MyGitHelp(); }},
    {"init", [](const auto &args) { MyGitInit(); }},
    {"add", [](const auto &args) { MyGitAdd(args); }},
    {"commit", [](const auto &args) { MyGitCommit(args); }},
    {"_erase", [](const auto &args) { MyGitErase(); }},
    {"status", [](const auto &args) { MyGitStatus(); }},
    {"log", [](const auto &args) { MyGitLog(); }},
    //{"checkout", [](const auto &args) { MyGitCheckout(val); }},
    //{"hash-object", [](const auto &args) { MyGitHashObject(val); }},
    {"diff", [](const auto &args) { MyGitDiff(); }},
    //{"branch", [](const auto &args) { MyGitBranch(val); }},
    //{"switch", [](const auto &args) { MyGitSwitch(val); }},
  };
  const std::string commandToRun = argv[1];
  if (const auto it = commands.find(commandToRun); it != commands.end()) {
    commands[commandToRun](arguments);
  } else {
    std::println(stderr, "Error: Command '{}' not found!", argv[1]);
    //get all keys from commands map by using views
    std::vector<std::string_view> possibleCommands = getCloseStrings(
      argv[1], commands | std::views::keys | std::ranges::to<std::vector<std::string_view> >());

    if (!possibleCommands.empty()) {
      std::println("Did you mean: '{}'?",
                   possibleCommands | std::views::join_with(std::string_view("' or '")) |
                   std::ranges::to<std::string>());
    }
  }
  /*
  std::string first = argv[1];
  if (first == "--help") {
    MyGitHelp();
  } else if (first == "init") {
    MyGitInit();
  } else if (first == "add") {
    if (argc < 3) {
      std::println(stderr, "Error: Not enough arguments provided.");
      return 1;
    }
    std::string second = argv[2];
    MyGitAdd(second);
  } else if (first == "commit") {
    if (argc < 3) {
      std::println(stderr, "Error: Not enough arguments provided.");
      return 1;
    }
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
    if (argc < 3) {
      std::println(stderr, "Error: Not enough arguments provided.");
      return 1;
    }
    std::string second = argv[2];
    MyGitCheckout(second);
  } else if (first == "hash-object") {
    if (argc < 3) {
      std::println(stderr, "Error: Not enough arguments provided.");
      return 1;
    }
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
    if (argc < 3) {
      std::println(stderr, "Error: Not enough arguments provided.");
      return 1;
    }
    std::string second = argv[2];
    MyGitSwitch(second);
  } else {
    std::println("Command {} not found", first);
    if (first == "erase") std::println("Did you mean '_erase'?");
  }*/

  return 0;
}
