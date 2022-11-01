#include "file_system_helper.h"

#include <iostream>
#include <fstream>
#include <sstream>

void FileSystemHelper::WriteFile(const std::string &filePath, const std::string &fileText) {
    std::ofstream myFile(filePath);
    std::stringstream textStream;
    textStream.str(fileText);
    myFile << textStream.rdbuf();
    myFile.close();
}
