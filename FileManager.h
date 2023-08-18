#pragma once

#include <fstream>
#include "Single.h"

class FileManager {
public:
    static void save(std::string fileName);
    static void load(std::string fileName);
};