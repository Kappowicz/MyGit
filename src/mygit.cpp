#include <iostream>
#include "mygit.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

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
      else 
        std::cout << "folder " << MAIN_FOLDER_NAME <<" failed to create" << std::endl;
      }
    
}

std::string calculateHash(std::string fileName) //something like djb2
{
  std::ifstream file(fileName, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file: " + fileName);
  }
    
    unsigned long long count = 1;
  char c;
  while (file.get(c)) {
      unsigned char value = static_cast<unsigned char>(c);
      
      count = ((count << 2) + count) + c;
  }

  std::cout << std::endl << "Hash for " << fileName << ": " << std::endl;
  std::cout << count << std::endl;
  std::cout << "Suma wszystkich cyfr w pliku wynosi: "
              << std::setw(20) << std::setfill('0') << count << std::endl;
  return std::to_string(count);
}
