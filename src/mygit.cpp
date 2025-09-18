#include <iostream>
#include "mygit.h"
#include <filesystem>
const std::string MAIN_FOLDER_NAME = ".mygit";
void printHelp() {
  std::cout << "this is my simple implementation of git \n start by initializing the repo with ./mygit init " << std::endl;
}

void initMyGit()
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
