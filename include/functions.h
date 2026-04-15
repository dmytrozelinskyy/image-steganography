#pragma once

#include "../include/bmp_header.h"
#include "../include/ppm_header.h"

#include <string>
#include <vector>

namespace bmp {
    void info(const std::string& path);

    void encrypt(const std::string& path, std::string msg);

    void decrypt(const std::string& path);

    void check(const std::string& path, const std::string& msg);
}

namespace ppm{

    void info(const std::string& path);

    void encrypt(const std::string &path, std::string msg);

    void decrypt(const std::string &path);

    void check(const std::string& path, const std::string& msg);
}


void help();