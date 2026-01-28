#include "../include/mygit.h"
#include <string>
#include <print>
#include <map>
#include <functional>
#include <ranges>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::println("start with 'mygit.exe --help': ");
    MyGitHelp();
    return 0;
  }

  //convert input chars to string_view for easier processing
  const std::vector<std::string_view> arguments(argv, argv + argc);

  //for small data sets its better to not use 'unordered' types
  std::map<std::string_view, std::function<void(std::vector<std::string_view>)> > commands = {
    {"--help", [](const auto &args) { MyGitHelp(); }},
    {"init", [](const auto &args) { MyGitInit(); }},
    {"add", [](const auto &args) { MyGitAdd(args); }},
    {"commit", [](const auto &args) { MyGitCommit(args); }},
    {"_erase", [](const auto &args) { MyGitErase(); }},
    {"status", [](const auto &args) { MyGitStatus(); }},
    {"log", [](const auto &args) { MyGitLog(); }},
    {"checkout", [](const auto &args) { MyGitCheckout(args); }},
    {"hash-object", [](const auto &args) { MyGitHashObject(args); }},
    {"diff", [](const auto &args) { MyGitDiff(); }},
    {"branch", [](const auto &args) { MyGitBranch(args); }},
    {"switch", [](const auto &args) { MyGitSwitch(args); }},
  };

  const std::string_view commandToRun = argv[1];
  if (const auto it = commands.find(commandToRun); it != commands.end()) {
    it->second(arguments);
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

  return 0;
}
