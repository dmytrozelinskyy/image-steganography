#pragma once

#include "../include/bmp_header.h"
#include "../include/ppm_header.h"
#include <iostream>

namespace bmp {
    bool readFromBMP(const std::string& path, BMP_FileHeader& fileHeader,
                     BMP_FileInfoHeader& fileInfoHeader, std::vector<unsigned char>& pixelData);

    bool writeToBMP(const std::string& path, std::vector<unsigned char>& pixelData,
                    int width, int height);
}

namespace ppm {
    bool readFromPPM(const std::string &path, PPM_FileHeader &ppm);
    bool writeToPPM(const std::string &path, PPM_FileHeader &ppmImage);
}