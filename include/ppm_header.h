#pragma once

#include <string>
#include <vector>
#include <cstdint>


enum class PPMType { P3, P6 };
/**
  @brief PPM file header metadata.
*/

struct PPM_FileHeader{
    PPMType type{PPMType::P6};
    uint32_t width{0};
    uint32_t height{0};
    uint8_t max_color_val{255};
    std::vector<unsigned char> image_data;
};