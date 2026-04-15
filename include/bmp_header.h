#pragma once

#include <cstdint>

/**
    @brief BMP file header (first 14 bytes of a .bmp file).
*/
#pragma pack(push, 1)
struct BMP_FileHeader {
    uint16_t fileType{0x4D42};
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t dataOffset{0};
};

/**
    @brief BMP info header (DIB header, 40 bytes).
*/
struct BMP_FileInfoHeader {
    uint32_t size{0};
    int32_t  width{0};
    int32_t  height{0};
    uint16_t planes{0};
    uint16_t bitCount{0};
    uint32_t compression{0};
    uint32_t imageSize{0};
    int32_t  xPixelsPerMeter{0};
    int32_t  yPixelsPerMeter{0};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
#pragma pack(pop)