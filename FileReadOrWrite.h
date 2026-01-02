#pragma once

#include "BMPHeaderStruct.h"
#include "PPMHeaderStruct.h"

namespace bmp {

    /**
     * @brief Reading data from the file (.bmp)
     * @function readFromBMP
     * @param path -> path of the file<br>
     * @param fileHeader -> object of BMP_FileHeader struct<br>
     * @param fileInfoHeader -> object of BMP_FileInfoHeader struct<br>
     * @param pixelData -> pixel data of the image(file)<br>
     * @details This function is used to read data [file header, file information header and pixel data] of the file(image)
     * @attention
     *  We are dividing bitCount by 8 to get bytes per pixel
     */

    auto readFromBMP(const std::string& path, BMP_FileHeader& fileHeader,
                     BMP_FileInfoHeader& fileInfoHeader, std::vector<unsigned char>& pixelData)->bool
    {
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return false;
        }

        /// Opening file
        std::ifstream file(path,std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Unable to open file! Path provided: " << path << std::endl;
            return false;
        }

        /// Reading File
        if (!file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BMP_FileHeader))) {
            std::cerr << "Failed to read BMP header." << std::endl;
            file.close();
            return false;
        }
        if (!file.read(reinterpret_cast<char*>(&fileInfoHeader), sizeof(BMP_FileInfoHeader))) {
            std::cerr << "Failed to read BMP information header." << std::endl;
            file.close();
            return false;
        }
        /// Checking whether bit count of the file is 24 and there is no compression [BMP format]
        if (fileInfoHeader.bitCount != 24 || fileInfoHeader.compression != 0) {
            std::cerr << "Unsupported BMP format." << std::endl;
            file.close();
            return false;
        }

        /// Calculating the total number of bytes needed to store the image data and resizing the pixelData to received value
        uint32_t imageSize = fileInfoHeader.width * fileInfoHeader.height * (fileInfoHeader.bitCount / 8);

        pixelData.resize(imageSize);

        /// Moving file read pointer to the start of the pixel data
        file.seekg(fileHeader.dataOffset, std::ios::beg);

        /// Reading pixel data
        if (!file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size())) {
            std::cerr << "Failed to read pixel data!" << std::endl;
            file.close();
            return false;
        }

        /// Closing file
        file.close();

        return true;
    }

    /**
     * @brief Writing data to the file (.bmp)
     * @function writeToBMP
     * @param path -> path of the file<br>
     * @param pixelData -> pixel data of the image(file)<br>
     * @param width -> file(image) width<br>
     * @param height -> file(image) height<br>
     * @details This function is used to write data into the file(image)
    */

    auto writeToBMP(const std::string& path, std::vector<unsigned char>& pixelData,
                    int width, int height)->bool
    {
        BMP_FileHeader fileHeader;
        BMP_FileInfoHeader fileInfoHeader;
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return false;
        }
        /// These variables should be assigned by us to pass all necessary data to create .bmp file
        fileHeader.fileType = 0x4d42; // 'BM'
        fileHeader.fileSize = sizeof(BMP_FileHeader) + sizeof(BMP_FileInfoHeader) + pixelData.size();
        fileHeader.reserved = 0;
        fileHeader.dataOffset= sizeof(BMP_FileHeader) + sizeof(BMP_FileInfoHeader);

        fileInfoHeader.size = sizeof(BMP_FileInfoHeader);
        fileInfoHeader.width = width;
        fileInfoHeader.height = height;
        fileInfoHeader.planes = 1;
        fileInfoHeader.bitCount = 24;
        fileInfoHeader.compression = 0;
        fileInfoHeader.imageSize = pixelData.size();
        fileInfoHeader.xPixelsPerMeter = 0;
        fileInfoHeader.yPixelsPerMeter = 0;
        fileInfoHeader.colorsUsed = 0;
        fileInfoHeader.colorsImportant = 0;

        /// Writing to the file [binary mode]
        std::ofstream new_file("..\\ImageStegonography\\bmp_encrypted_file.bmp", std::ios::binary);
        if(!new_file){
            std::cerr << "Error loading file! Path provided: " << path << std::endl;
            return false;
        }

        new_file.write(reinterpret_cast<const char*>(&fileHeader),sizeof(fileHeader));
        new_file.write(reinterpret_cast<const char*>(&fileInfoHeader), sizeof(fileInfoHeader));
        new_file.write(reinterpret_cast<const char*>(pixelData.data()), pixelData.size());

        /// Closing file
        new_file.close();

        return true;
    }
}

namespace ppm {
    /**
     * @brief Reading data from the file (.ppm)
     * @function readPPMImage
     * @param path -> path of the file<br>
     * @param ppm -> object of PPM_FileHeader struct<br>
     * @details This function is used to read data [file magic number, file width and height, file max color value and image(pixel) data] of the file(image)
     * @attention
     *  ! Image data size differs here from .bmp !<br>
     *  We need to count number of pixels, so we will multiply image width with height and then multiply result with 3 (3 -> 3 values R, G, B)
     */

    auto readPPMImage(const std::string &path, PPM_FileHeader &ppm) -> bool {
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return false;
        }
        /// Opening file
        std::ifstream ppm_file(path,std::ios::binary);
        if (!ppm_file.is_open()) {
            std::cerr << "Unable to open file! Path provided: " << path << std::endl;
            return false;
        }

        /// Reading magic number(type)
        ppm_file >> ppm.magic_number;

        /// Checking magic number(type) [P3 - plain text, P6 - binary]
        if (ppm.magic_number != "P6") {
            std::cerr << "Unsupported BMP format." << std::endl;
            ppm_file.close();
            return false;
        }

        /// Reading other data
        ppm_file >> ppm.width >> ppm.height >> ppm.max_color_val;

        /// Skipping whitespace
        ppm_file.ignore();

        /// Calculating number of pixels and resizing image(pixel) data to this size
        int numberOfPixels = ppm.width * ppm.height;
        ppm.image_data.resize(numberOfPixels * 3);

        /// Reading pixel data from the file(image)
        ppm_file.read(reinterpret_cast<char *>(&ppm.image_data[0]), ppm.image_data.size());
        if (!ppm_file) {
            std::cout << "Error while reading image(pixel) data!" << std::endl;
            return false;
        }

        /// Closing file
        ppm_file.close();

        return true;
    }

    /**
     * @brief Writing data to the file (.ppm)
     * @function writeToBMP
     * @param path -> path of the file<br>
     * @param ppmImage -> object of PPM_FileHeader
     * @details This function is used to write data into the file(image)
    */

    auto writeToPPM(const std::string &path, PPM_FileHeader &ppmImage) -> bool {
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return false;
        }

        /// Writing into the file
        std::ofstream new_file("..\\ImageStegonography\\ppm_encrypted_file.ppm", std::ios::binary);
        if (!new_file) {
            std::cerr << "Error loading file!" << std::endl;
            return false;
        }

        /// These information shoul be assigned by us pass all necessary data to create .ppm file
        new_file << "P6\n" << ppmImage.width << " " << ppmImage.height << "\n";
        new_file << ppmImage.max_color_val << "\n"; // ! add '\n' elements after each input

        /// Writing image(pixel) data into the file
        new_file.write(reinterpret_cast<const char *>(ppmImage.image_data.data()), ppmImage.image_data.size());
        if (!new_file) {
            std::cerr << "Error writing image(pixel) data to the file!" << std::endl;
            return false;
        }

        /// Closing file
        new_file.close();

        return true;
    }
}




/**
     * @SourceOfInformation
     * <h3>Internet:</h3>
     *  On the Internet, I found an information which tells that to read pixel data of the .bmp files, we need to move the file read pointer to the beginning of the pixel data.
     * <h3>ChatGPT(OpenAI):</h3>
     * <h4>1) How to move file read pointer to the beginning of the pixel data in C++?</h4>
     * &emsp; Me: Tell me about .bmp file type and what main properties it has<br><br>
     *
     * &emsp; ChatGPT: To move the file read pointer to the beginning of the pixel data in a file,
     *                 you'll typically need to know where the pixel data starts. In many file formats,
     *                 such as BMP or PNG, the pixel data starts after a header of a specific size.
     *                 Here, I'll demonstrate how to do this for a BMP file as an example.<br>
     *                 &emsp;<b>Steps to Move File Read Pointer to Pixel Data</b><br>
     *                 &emsp;Open the File: <br>
     *                 <p> &emsp;&emsp;- Use std::ifstream to open the file in binary mode. <br></p>
     *                 &emsp;<b>Read the Header: </b><br>
     *                 <p> &emsp;&emsp;- For BMP files, the header size is generally fixed. You can read the header to determine where the pixel data starts.<br></p>
     *                 &emsp;<b>Move the Pointer: </b>:<br>
     *                 <p> &emsp;&emsp;- Use seekg to move the file pointer to the offset where the pixel data begins.<br></p>
     *                 &emsp;<b>Sample Code: </b>:<br>
     * @code    int main() {
     *          std::ifstream file("example.bmp", std::ios::binary);
     *           if (!file) {
     *               std::cerr << "Error opening file!" << std::endl;
     *               return 1;
     *           }
     *
     *           // Read the BMP file header
     *           BMPHeader bmpHeader;
     *           file.read(reinterpret_cast<char*>(&bmpHeader),
     *           sizeof(BMPHeader));
     *
     *           // Check if it's a BMP file by looking at the magic
     *           number
     *           if (bmpHeader.bfType != 0x4D42) { // 'BM' in
     *           little-endian
     *               std::cerr << "Not a BMP file!" << std::endl;
     *               return 1;
     *           }
     *
     *           // Read the BMP info header
     *           BMPInfoHeader bmpInfoHeader;
     *           file.read(reinterpret_cast<char*>(&bmpInfoHeader),
     *           sizeof(BMPInfoHeader));
     *
     *           // Move the file pointer to the beginning of the
     *           pixel data
     *           file.seekg(bmpHeader.bfOffBits, std::ios::beg);
     *
     *           // Now you can read the pixel data
     *           // Example: Read the first pixel (assuming 24 bits per
     *           pixel and no compression)
     *           unsigned char pixel[3];
     *           file.read(reinterpret_cast<char*>(pixel), 3);
     *
     *           std::cout << "First pixel (BGR): ("
     *                     << static_cast<int>(pixel[0]) << ", "
     *                     << static_cast<int>(pixel[1]) << ", "
     *                     << static_cast<int>(pixel[2]) << ")"
     *                     << std::endl;
     *
     *           file.close();
     *           return 0;
     *       }<br>
     *
     * <h4>2) How to move file read pointer to the beginning of the pixel data in C++?</h4>
     * &emsp; Me: Do we need to add new line(that we were ignoring while reading .ppm file(image)) somewhere when we are writing into .ppm file?<br><br>
     *
     * &emsp; ChatGPT: Yes, when you are writing into a .ppm (Portable Pixel Map) file,
     *                 you need to ensure that you format it correctly to maintain
     *                 the integrity of the image data. Here are the key points to consider:<br>
     *                      &emsp;<b>1. Header Information:</b> The .ppm format requires specific header
     *                            information at the beginning of the file. This includes
     *                            the magic number (P3 for ASCII format), width, height,
     *                            and maximum color value.<br>
     *                      &emsp;<b>2. Pixel Data:</b> After the header, the pixel data follows.
     *                            Each pixel's RGB values are typically represented in ASCII format (if using P3)
     *                            or binary format (if using P6).<br>
     *                      &emsp;<b>3. Formatting Considerations:</b> <br>
     *                      &emsp;&emsp;<b>Whitespace:</b> There should be whitespace (spaces or newline characters)
     *                                              to separate the values in the header and between pixel values. <br>
     *                      &emsp;&emsp;<b>Newlines:</b> Especially in P3 format (ASCII), after every set of RGB values for a pixel,
     *                                            there should be a newline character to indicate the end of that line of pixels. <br>
     *                      &emsp;<b>4. End of File:</b> Ensure the file ends with a newline character. This is generally considered good practice
     *                               in text files to prevent issues with certain text editors or parsers.<br>
     */