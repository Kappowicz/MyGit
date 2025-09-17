#include "mygit.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <cstring>
const std::string MAIN_FOLDER_NAME = ".mygit";
int main (int argc, char *argv[]) {
  if (argc == 1)
  {
    std::cout << "start with 'mygit.exe --help'" << std::endl;
    printHelp();
    return 0;
  }

  std::string first = "siema"; // argv[1];
  if (first == "--help")
  {
    printHelp();
  }
  else if (first == "init")
  {
    if (std::filesystem::is_directory(MAIN_FOLDER_NAME))
    {
      std::cout << "folder " << MAIN_FOLDER_NAME <<" exists" << std::endl;
    }
    else {
      if (std::filesystem::create_directory(MAIN_FOLDER_NAME))
      {
        std::cout << "folder " << MAIN_FOLDER_NAME <<" created" << std::endl;
      }
      else {
        std::cout << "folder " << MAIN_FOLDER_NAME <<" failed to create" << std::endl;
      }
    }
  }

  return 0;
}
