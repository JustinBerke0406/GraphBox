#pragma once

#include <fstream>
#include <filesystem>
#include "Single.h"

class FileManager {
    static std::string fileName(std::string path);
public:
    static void save(std::string fileName);
    static void load(std::string fileName);

    static void saveDialog();
    static void loadDialog();
};