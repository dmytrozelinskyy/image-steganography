#include "../include/file_io.h"
#include <iostream>
#include <fstream>

namespace bmp {

    auto readFromBMP(const std::string &path, BMP_FileHeader &fileHeader,
                     BMP_FileInfoHeader &fileInfoHeader, std::vector<unsigned char> &pixelData)
    {
        std::ifstream file(path,std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Unable to open file! Path provided: " << path << '\n';
            return false;
        }

        if (!file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BMP_FileHeader))) {
            std::cerr << "Failed to read BMP header.\n";
            return false;
        }

        if (!file.read(reinterpret_cast<char*>(&fileInfoHeader), sizeof(BMP_FileInfoHeader))) {
            std::cerr << "Failed to read BMP information header.\n";
            return false;
        }

        if (fileInfoHeader.bitCount != 24 || fileInfoHeader.compression != 0) {
            std::cerr << "Unsupported BMP format.\n";
            return false;
        }

        uint32_t imageSize = static_cast<uint32_t>(
            static_cast<uint64_t>(fileInfoHeader.width) *
            static_cast<uint64_t>(fileInfoHeader.height) *
            (fileInfoHeader.bitCount / 8)
        );
        pixelData.resize(imageSize);

        file.seekg(fileHeader.dataOffset, std::ios::beg);

        if (!file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size())) {
            std::cerr << "Failed to read pixel data!\n";
            return false;
        }

        return true;
    }

    auto writeToBMP(const std::string& path, std::vector<unsigned char>& pixelData,
                    int width, int height)
    {
        if (width <= 0 || height <= 0) {
            std::cerr << "Invalid image dimensions.\n";
            return false;
        }

        BMP_FileHeader fileHeader;
        BMP_FileInfoHeader fileInfoHeader;

        fileHeader.fileSize      = static_cast<uint32_t>(sizeof(BMP_FileHeader) + sizeof(BMP_FileInfoHeader) + pixelData.size());
        fileHeader.dataOffset    =  static_cast<uint32_t>(sizeof(BMP_FileHeader) + sizeof(BMP_FileInfoHeader));

        fileInfoHeader.size      = sizeof(BMP_FileInfoHeader);
        fileInfoHeader.width     = width;
        fileInfoHeader.height    = height;
        fileInfoHeader.bitCount  = 24;
        fileInfoHeader.imageSize = static_cast<uint32_t>(pixelData.size());

        std::ofstream file(path, std::ios::binary);
        if(!file){
            std::cerr << "Error loading file! Path provided: " << path << '\n';
            return false;
        }

        file.write(reinterpret_cast<const char*>(&fileHeader),sizeof(fileHeader));
        file.write(reinterpret_cast<const char*>(&fileInfoHeader), sizeof(fileInfoHeader));
        file.write(reinterpret_cast<const char*>(pixelData.data()), pixelData.size());

        if (!file) {
            std::cerr << "Error writing BMP data to: " << path << '\n';
            return false;
        }

        return true;
    }
}

namespace ppm {
    auto readFromPPM(const std::string &path, PPM_FileHeader &ppmHeader)
    {
        std::ifstream file(path,std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Unable to open file! Path provided: " << path << '\n';
            return false;
        }

        std::string magic;
        file >> magic;
        if (magic != "P6") {
            std::cerr << "Unsupported PPM format.\n";
            return false;
        }
        ppmHeader.type = PPMType::P6;

        file >> ppmHeader.width >> ppmHeader.height >> ppmHeader.max_color_val;
        file.ignore();

        size_t numberOfPixels = static_cast<size_t>(ppmHeader.width) * static_cast<size_t>(ppmHeader.height);
        ppmHeader.image_data.resize(numberOfPixels * 3);

        file.read(reinterpret_cast<char *>(&ppmHeader.image_data[0]), ppmHeader.image_data.size());
        if (!file) {
            std::cout << "Error while reading pixel data!\n";
            return false;
        }

        return true;
    }

    auto writeToPPM(const std::string &path, PPM_FileHeader &ppmImage)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "Error loading file!\n";
            return false;
        }

        file << "P6\n" << ppmImage.width << " " << ppmImage.height <<'\n' << static_cast<int>(ppmImage.max_color_val) << '\n';

        file.write(reinterpret_cast<const char *>(ppmImage.image_data.data()), ppmImage.image_data.size());
        if (!file) {
            std::cerr << "Error writing image(pixel) data to the file!\n";
            return false;
        }

        return true;
    }
}
