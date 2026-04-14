#pragma once

#include "../include/bmp_header.h"
#include "../include/ppm_header.h"
#include <iostream>

namespace bmp {
    auto readFromBMP(const std::string& path, BMP_FileHeader& fileHeader,
                     BMP_FileInfoHeader& fileInfoHeader, std::vector<unsigned char>& pixelData);

    auto writeToBMP(const std::string& path, std::vector<unsigned char>& pixelData,
                    int width, int height);
}

namespace ppm {

    auto readFromPPM(const std::string &path, PPM_FileHeader &ppm);
    auto writeToPPM(const std::string &path, PPM_FileHeader &ppmImage);
}