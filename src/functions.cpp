#include "../include/functions.h"
#include "../include/bmp_header.h"
#include "../include/ppm_header.h"
#include "../include/file_io.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <cstdint>

namespace bmp
{
    void info(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "Unable to open file! Path provided: " << path << '\n';
            return;
        }

        BMP_FileHeader fileHeader;
        BMP_FileInfoHeader fileInfoHeader;

        if (!file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BMP_FileHeader))) {
            std::cerr << "Failed to read BMP header.\n";
            return;
        }

        if (fileHeader.fileType != 0x4D42) {
            std::cerr << "Not a valid BMP file: " << path << '\n';
            return;
        }

        if (!file.read(reinterpret_cast<char*>(&fileInfoHeader), sizeof(fileInfoHeader))) {
            std::cerr << "Failed to read BMP info header.\n";
            return;
        }

        std::cout << "File Type:    BM (0x4D42)\n"
                  << "File Size:         " << fileHeader.fileSize         << " bytes\n"
                  << "Image Header Size: " << fileInfoHeader.size         << " bytes\n"
                  << "Image Width:       " << fileInfoHeader.width        << " px\n"
                  << "Image Height:      " << fileInfoHeader.height       << " px\n"
                  << "Bits Per Pixel:    " << fileInfoHeader.bitCount     << " \n"
                  << "Compression:       " << fileInfoHeader.compression  << "\n"
                  << "Image Data Size:   " << fileInfoHeader.imageSize    << " bytes\n";
    }

    void encrypt(const std::string& path, std::string msg)
    {
        BMP_FileHeader fileHeader;
        BMP_FileInfoHeader fileInfoHeader;
        std::vector<unsigned char> pixelData;

        if (!bmp::readFromBMP(path, fileHeader, fileInfoHeader, pixelData)) {
            return;
        }

        auto toBits = [](auto value, int numBits) {
            std::string bitString;
            bitString.reserve(numBits);
            for (int i = numBits - 1; i >= 0; --i)
                bitString += ('0' + ((value >> i) & 1));
            return bitString;
        };

        std::string bits = toBits(static_cast<uint32_t>(msg.size()), 32);
        for (unsigned char c : msg)
            bits += toBits(c, 8);

        size_t capacity = static_cast<size_t>(fileInfoHeader.width) * static_cast<size_t>(fileInfoHeader.height) * 6;
        if (bits.size() > capacity) {
            std::cerr << "Message too large for this image. "
                      << "Max: " << capacity / 8 << " bytes, "
                      << "Got: " << msg.size() << " bytes.\n";
            return;
        }

        size_t bitIndex = 0;

        auto changeBit = [&](std::bitset<8>& b, int pos) {
            if (bitIndex >= bits.size()) return;
            b.set(pos, bits[bitIndex++] == '1');
        };

        for (int y = fileInfoHeader.height - 1; y >= 0; --y) {
            for (int x = 0; x < fileInfoHeader.width; ++x) {
                if(bitIndex >= bits.size())
                {
                    std::string outPath = path.substr(0, path.rfind('.')) + "_encrypted.bmp";
                    if (!bmp::writeToBMP(outPath, pixelData, fileInfoHeader.width, fileInfoHeader.height)) {
                        std::cerr << "Failed to write encrypted file.\n";
                        return;
                    }
                    std::cout << "Message encrypted successfully into: " << outPath << '\n';
                    return;
                }

                size_t index = (static_cast<size_t>(x) + static_cast<size_t>(y) * fileInfoHeader.width) * 3;
                std::bitset<8> blue = pixelData[index];
                std::bitset<8> green = pixelData[index + 1];
                std::bitset<8> red = pixelData[index + 2];

                changeBit(red, 1); changeBit(red, 0);
                changeBit(green, 1); changeBit(green, 0);
                changeBit(blue, 1); changeBit(blue, 0);

                pixelData[index] = static_cast<unsigned char>(blue.to_ulong());
                pixelData[index + 1] = static_cast<unsigned char>(green.to_ulong());
                pixelData[index + 2] = static_cast<unsigned char>(red.to_ulong());
            }
        }
        std::cerr << "Encryption failed - ran out of pixels unexpectedly.\n";
    }

    void decrypt(const std::string& path)
    {
        BMP_FileHeader fileHeader;
        BMP_FileInfoHeader fileInfoHeader;
        std::vector<unsigned char> pixelData;

        if (!bmp::readFromBMP(path, fileHeader, fileInfoHeader, pixelData)) {
            return;
        }

        std::string bitString;
        bitString.reserve(pixelData.size() * 2);

        auto extractBits = [&](std::bitset<8> b) {
            bitString += ('0' + (int)b[1]);
            bitString += ('0' + (int)b[0]);
        };

        for (int y = fileInfoHeader.height - 1; y >= 0; --y) {
            for (int x = 0; x < fileInfoHeader.width; ++x) {
                size_t index = (static_cast<size_t>(x) + static_cast<size_t>(y) * fileInfoHeader.width) * 3;

                extractBits(std::bitset<8>(pixelData[index]));     // blue
                extractBits(std::bitset<8>(pixelData[index + 1])); // green
                extractBits(std::bitset<8>(pixelData[index + 2])); // red
            }
        }

        if (bitString.size() < 32) {
            std::cerr << "Image too small to encrypt message.\n";
            return;
        }

        uint32_t msgLength = std::bitset<32>(bitString.substr(0, 32)).to_ulong();

        if (msgLength == 0 || msgLength * 8 + 32 > bitString.size()) {
            std::cerr << "No valid message found in image.\n";
            return;
        }

        std::string msg;
        msg.reserve(msgLength);
        for (uint32_t i = 0; i < msgLength; ++i) {
            std::string byte = bitString.substr(32 + i * 8, 8);
            msg += static_cast<char>(std::bitset<8>(byte).to_ulong());
        }

        std::cout << "Decrypted message: " << msg << '\n';

    }

    void check(const std::string& path, const std::string& msg)
    {
        BMP_FileHeader fileHeader;              // <<File Header[fileType, fileSize, dataOffset]>>
        BMP_FileInfoHeader fileInfoHeader;      // <<File Header Information[size, width, height, bitCount, compression, ...]>>
        std::vector<unsigned char> pixelData;

        if (!bmp::readFromBMP(path, fileHeader, fileInfoHeader, pixelData)) {
            return;
        }

        size_t capacity = (static_cast<size_t>(fileInfoHeader.width) * fileInfoHeader.height * 6) / 8;
        size_t requiredCapacity = msg.size() + 4;

        std::cout << "Message size: " << msg.size() << " bytes.\n"
        << "Image capacity: " << capacity << " bytes.\n"
        << "Required capacity: " << requiredCapacity << " bytes.\n";

        if (requiredCapacity > capacity) {
            std::cerr << "Message too large to be encrypted in this image.\n";
        } else {
            std::cout << "Message fits - image has: " << (capacity - requiredCapacity) << " bytes to sapre.\n";
        }
    }
}

namespace ppm {
    void info(const std::string& path)
    {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Unable to open file! Path provided: " << path << std::endl;
            return;
        }

        std::string magic;
        uint32_t width, height;
        int maxColor;

        if (!(file >> magic >> width >> height >> maxColor)) {
            std::cerr << "Failed to read PPM header.\n";
            return;
        }

        if (magic != "P6" && magic != "P3") {
            std::cerr << "Invalid PPM file: " << path << '\n';
            return;
        }

        std::cout << "Magic Number(Type): " << magic << '\n'
        << "Width: " << width << " px.\n"
        << "Height: " << height << " px.\n"
        << "Max Color Value: " << maxColor << '\n';
    }

    void encrypt(const std::string& path, std::string msg)
    {
        PPM_FileHeader imageHeader;

        if (!ppm::readFromPPM(path, imageHeader)) {
            return;
        }

        auto toBits = [](auto value, int numBits) {
            std::string bitString;
            bitString.reserve(numBits);
            for (int i = numBits - 1; i >= 0; --i)
                bitString += ('0' + ((value >> i) & 1));
            return bitString;
        };

        std::string bits = toBits(static_cast<uint32_t>(msg.size()), 32);
        for (unsigned char c : msg)
            bits += toBits(c, 8);

        size_t capacity = static_cast<size_t>(imageHeader.width) * static_cast<size_t>(imageHeader.height) * 6;
        if (bits.size() > capacity) {
            std::cerr << "Message too large for this image. "
                      << "Max: " << capacity / 8 << " bytes, "
                      << "Got: " << msg.size() << " bytes.\n";
            return;
        }

        size_t bitIndex = 0;

        auto changeBit = [&](std::bitset<8>& b, int pos) {
            if (bitIndex >= bits.size()) return;
            b.set(pos, bits[bitIndex++] == '1');
        };

        for (uint32_t y = 0; y < imageHeader.height; ++y) {
            for (uint32_t x = 0; x < imageHeader.width; ++x) {
                if(bitIndex >= bits.size())
                {
                    std::string outPath = path.substr(0, path.rfind('.')) + "_encrypted.bmp";
                    if (!ppm::writeToPPM(outPath, imageHeader)) {
                        std::cerr << "Failed to write encrypted file.\n";
                        return;
                    }
                    std::cout << "Message encrypted successfully into: " << outPath << '\n';
                    return;
                }

                size_t index = (x + y * imageHeader.width) * 3;
                std::bitset<8> blue = imageHeader.image_data[index];
                std::bitset<8> green = imageHeader.image_data[index + 1];
                std::bitset<8> red = imageHeader.image_data[index + 2];

                changeBit(red, 1); changeBit(red, 0);
                changeBit(green, 1); changeBit(green, 0);
                changeBit(blue, 1); changeBit(blue, 0);

                imageHeader.image_data[index] = static_cast<unsigned char>(blue.to_ulong());
                imageHeader.image_data[index + 1] = static_cast<unsigned char>(green.to_ulong());
                imageHeader.image_data[index + 2] = static_cast<unsigned char>(red.to_ulong());
            }
        }
        std::cerr << "Encryption failed - ran out of pixels unexpectedly.\n";
    }

    void decrypt(const std::string& path)
    {
        PPM_FileHeader imageHeader;

        if (!ppm::readFromPPM(path, imageHeader)) {
            return;
        }

        std::string bitString;
        bitString.reserve(imageHeader.image_data.size() * 2);

        auto extractBits = [&](std::bitset<8> b) {
            bitString += ('0' + (int)b[1]);
            bitString += ('0' + (int)b[0]);
        };

        for (uint32_t y = 0; y < imageHeader.height; ++y) {
            for (uint32_t x = 0; x < imageHeader.width; ++x) {
                size_t index = (x + y * imageHeader.width) * 3;

                extractBits(std::bitset<8>(imageHeader.image_data[index]));     // blue
                extractBits(std::bitset<8>(imageHeader.image_data[index + 1])); // green
                extractBits(std::bitset<8>(imageHeader.image_data[index + 2])); // red
            }
        }

        if (bitString.size() < 32) {
            std::cerr << "Image too small to encrypt message.\n";
            return;
        }

        uint32_t msgLength = std::bitset<32>(bitString.substr(0, 32)).to_ulong();

        if (msgLength == 0 || msgLength * 8 + 32 > bitString.size()) {
            std::cerr << "No valid message found in image.\n";
            return;
        }

        std::string msg;
        msg.reserve(msgLength);
        for (uint32_t i = 0; i < msgLength; ++i) {
            std::string byte = bitString.substr(32 + i * 8, 8);
            msg += static_cast<char>(std::bitset<8>(byte).to_ulong());
        }

        std::cout << "Decrypted message: " << msg << '\n';

    }

    void check(const std::string& path, const std::string& msg)
    {
        PPM_FileHeader imageHeader;

        if (!ppm::readFromPPM(path, imageHeader)) {
            return;
        }

        size_t capacity = (static_cast<size_t>(imageHeader.width) * imageHeader.height * 6) / 8;
        size_t requiredCapacity = msg.size() + 4;

        std::cout << "Message size: " << msg.size() << " bytes.\n"
        << "Image capacity: " << capacity << " bytes.\n"
        << "Required capacity: " << requiredCapacity << " bytes.\n";

        if (requiredCapacity > capacity) {
            std::cerr << "Message too large to be encrypted in this image.\n";
        } else {
            std::cout << "Message fits - image has: " << (capacity - requiredCapacity) << " bytes to sapre.\n";
        }
    }
}

void help()
{
    std::cout << "\t    <Help>\n";
    std::cout << " Supported image file extensions" << std::endl;
    std::cout << "  .bmp\t" << std::endl;
    std::cout << "  .ppm\t" << std::endl;
    std::cout << " Unsupported image file extensions" << std::endl;
    std::cout << "  .gif" << std::endl;
    std::cout << "  .jpeg" << std::endl;
    std::cout << "  .jpg" << std::endl;
    std::cout << " Usage instructions" << std::endl;
    std::cout << "  * Do not try to input unsupported format files or flags, it will result in error!\t" << std::endl;
    std::cout << "  * If You want to re-encrypt another text to the image with already encrypted text, just use flag -e\t" << std::endl;
    std::cout << "  * Carefully check provided image location path\t" << std::endl;
    std::cout << "  * When You are providing both path and msg, firstly provide path and then message\t" << std::endl;
    std::cout << "  * Message argument can ve provided in quotes \" \" for it to be with spaces, e.g. \"Hello, My Dear Friend!\"\t" << std::endl;
    std::cout << "  * In case of some bugs or errors contact us on +48519578025\t" << std::endl;
    std::cout << " Supported flags list: " << std::endl;
    std::cout << "  -i" << std::endl;
    std::cout << "  -e" << std::endl;
    std::cout << "  -d" << std::endl;
    std::cout << "  -c" << std::endl;
    std::cout << "  -h" << std::endl;
}