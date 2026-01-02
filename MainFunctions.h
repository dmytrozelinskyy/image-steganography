#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstdint>

#include "BMPHeaderStruct.h"
#include "PPMHeaderStruct.h"
#include "FileReadOrWrite.h"



namespace bmp {
    /**
    * @brief Get detailed information about the file(image).
    * @function info
    *
    * @param path [const std::string& path]<br>
    *        Path to the file(image) we want to get info of.
    * @flags -i <i>OR</i> --info
    * @details This function is used to get varity of details about the specified file(image), such as:<br>
    *              &emsp;&emsp;- File Type<br>
    *              &emsp;&emsp;- File Size<br>
    *              &emsp;&emsp;- Image Header Size<br>
    *              &emsp;&emsp;- Image Width<br>
    *              &emsp;&emsp;- Image Height<br>
    *              &emsp;&emsp;- Size of the image data
    * @attention<br>
    * <h3> *File Header</h3>
    * Data within a file that contains identifying
    * information about the file and possibly metadata
    * with information about the file contents.<br><br>
    * <h3> *0x4D42</h3>
    * 0x4D42 -> "BM" in hexadecimal<br><br>
    * <h3> *reinterpret_cast(char*)</h3>
    * I am using reinterpret_cast(char*) to read bytes into BMP_FileHeader and BMP_FileInfoHeader structs variables.<br>
    * More information about this in [BMPHeaderStruct.h] header file
    * */

    auto info(const std::string& path)->void{
        BMP_FileHeader fileHeader;              // <<File Header[fileType, fileSize, dataOffset]>>
        BMP_FileInfoHeader fileInfoHeader;      // <<File Header Information[size, width, height, bitCount, compression, ...]>>

        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }

        /// Opening file in binary mode
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "Unable to open file! Path provided: " << path << std::endl;
            return;
        }

        /// Reading file header
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BMP_FileHeader));

        /// Checking if provided file is in BMP format
        if (fileHeader.fileType != 0x4D42) {//(0x4D42 -> "BM" in hexadecimal)
            std::cerr << "Provided file is not a valid BMP file! Path provided: " << path << std::endl;
            return;
        }

        /// Reading File Header Information
        file.read(reinterpret_cast<char*>(&fileInfoHeader), sizeof(fileInfoHeader));

        /// Printing Received Information
        std::cout << "File Type: " << fileHeader.fileType << std::endl;
        std::cout << "File Size: " << fileHeader.fileSize << " bytes." << std::endl;

        std::cout << "Image Header Size: " << fileInfoHeader.size << std::endl;
        std::cout << "Image Width: " << fileInfoHeader.width << std::endl;
        std::cout << "Image Height: " << fileInfoHeader.height << std::endl;
        std::cout << "Size of the image data: " << fileInfoHeader.imageSize << " bytes." << std::endl;

        /// Closing File
        file.close();
    }

    /**
    * @brief Encrypt message into image
    * @function encrypt
    *
    * @param path -> path of the file(image)
    * @param msg -> message that should be encrypted
    * @flags -e <i>OR</i> -encrypt
    * @details This function is used to encrypt provided message into the image
    * @attention * .bmp file stores data starting from the bottom-left corner<br>
     *           * std::bitset<8> stores LSB on the right side, so for example (7,6,5,4,3,2,1,0) bitset will have a LSB at position 0<br>
     *           * Changing each 2 LSB in .bmp file (image)
    * */

    auto encrypt(const std::string& path, std::string msg) -> void{
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }

        BMP_FileHeader fileHeader;              // <<File Header[fileType, fileSize, dataOffset]>>
        BMP_FileInfoHeader fileInfoHeader;      // <<File Information Header[size, width, height, bitCount, compression, ...]>>
        std::vector<unsigned char> pixelData;   // vector of image(pixel) data

        /// Converting message into binary string
        std::bitset<8> string_to_b[100];
        for (int i = 0; i < msg.size(); ++i) string_to_b[i] = std::bitset<8>(msg[i]);
        auto long_string = std::string();
        for (int i = 0; i < msg.size(); ++i)
            for (int j = 7; j >= 0; --j)
                long_string += std::to_string(string_to_b[i][j]);

        /// Reading File
        if(!bmp::readFromBMP(path, fileHeader, fileInfoHeader, pixelData)){
            return;
        }

        /// Checking if image(pixel) data is not empty
        if(pixelData.empty()){
            std::cout << "Error! Image(pixel) data is empty!" << std::endl;
            return;
        }

        for (int y = fileInfoHeader.height - 1; y >= 0; --y) { //starting from the bottom-left corner
            for (int x = 0; x < fileInfoHeader.width; ++x) { //going through each pixel in line
                /// Checking if long_string is empty (end of the looping, message is encrypted)
                if(long_string.empty()){
                    bmp::writeToBMP(path, pixelData, fileInfoHeader.width, fileInfoHeader.height);
                    std::ofstream message_log("..\\ImageStegonography\\message_log.txt", std::ios::app);
                    message_log << x;
                    message_log.close();
                    std::cout << "Message is successfully encrypted into bmp_encrypted_file.bmp!" << std::endl;
                    return;
                }

                /// Calculating index to access image(pixel) data and extracting RGB values from each pixel

                int index = (x + y * fileInfoHeader.width) * 3;
                unsigned char blue = pixelData[index];
                unsigned char green = pixelData[index + 1];
                unsigned char red = pixelData[index + 2];

                /// Converting RGB values into bitsets
                std::bitset<8> red_value(red);
                std::bitset<8> green_value((char)green);
                std::bitset<8> blue_value((char)blue);

                /// Lambda function to change bit at some index in bitsets (bit representaion of RGB values)

                auto change_value = [&long_string](std::bitset<8>& bitset, int index)->void{
                    if (index < 0 || index >= bitset.size()) {
                        std::cerr << "Error! Index: " << index << " is out of bounds for bitset of size: " << bitset.size() <<  std::endl;
                        return;
                    }
                    if(long_string.empty()){
                        return;
                    }
                    if(long_string[0] == '0') bitset.set(index, 0);
                    else bitset.set(index, 1);
                    long_string.erase(0, 1);
                };

                change_value(red_value, 1);
                change_value(red_value, 0);
                change_value(green_value, 1);
                change_value(green_value, 0);
                change_value(blue_value, 1);
                change_value(blue_value, 0);

                /// Changing each RGB values in image(pixel) data
                pixelData[index] = static_cast<unsigned char>(blue_value.to_ulong());
                pixelData[index + 1] = static_cast<unsigned char>(green_value.to_ulong());
                pixelData[index + 2] = static_cast<unsigned char>(red_value.to_ulong());
            }
        }
    }


    /**
    * @brief Decrypt message from image
    * @function decrypt
    *
    * @param path -> path of the file(image)
    * @flags -d <i>OR</i> -decrypt
    * @details This function is used to decrypt message from the image
    * @attention * .bmp file stores data starting from the bottom-left corner<br>
    *            * std::bitset<8> stores LSB on the right side, so for example (7,6,5,4,3,2,1,0) bitset will have a LSB at position 0<br>
    * */

    auto decrypt(const std::string& path)->void{
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }

        /// Getting size of long_string
        std::string message, line;
        std::fstream message_log("..\\ImageStegonography\\message_log.txt");
        while(std::getline(message_log, line)){
            message += line;
        }
        message_log.close();

        /// Deleting the content of the message log file
        std::ofstream mf("..\\ImageStegonography\\message_log.txt", std::ios::out | std::ios::trunc);
        mf.close();

        /// Opening file
        std::ifstream file(path,
                           std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Failed to open file! Path provided: " << path << std::endl;
            return;
        }

        BMP_FileHeader fileHeader;              // <<File Header[fileType, fileSize, dataOffset]>>
        BMP_FileInfoHeader fileInfoHeader;      // <<File Header Information[size, width, height, bitCount, compression, ...]>>
        std::vector<unsigned char> pixelData;

        /// Reading File Header
        if (!file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BMP_FileHeader))) {
            std::cerr << "Failed to read BMP Header." << std::endl;
            file.close();
            return;
        }

        /// Reading File Header Information
        if (!file.read(reinterpret_cast<char*>(&fileInfoHeader), sizeof(BMP_FileInfoHeader))) {
            std::cerr << "Failed to read BMP Information Header." << std::endl;
            file.close();
            return;
        }

        /// Checking whether bit count of the file is 24 and there is no compression [BMP format]
        if (fileInfoHeader.bitCount != 24 || fileInfoHeader.compression != 0) {
            std::cerr << "Unsupported BMP format." << std::endl;
            file.close();
            return;
        }

        /// Moving file pointer to the beginning of the image(pixel) data
        file.seekg(fileHeader.dataOffset, std::ios::beg);

        /// Resizing image(pixel) data
        pixelData.resize(fileInfoHeader.imageSize);
        if (!file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size())) {
            std::cerr << "Failed to read pixel data!" << std::endl;
            file.close();
            return;
        }
        file.close();


        auto res = std::string();
        for (int y = fileInfoHeader.height - 1; y >= 0; --y) {
            for (int x = 0; x < fileInfoHeader.width; ++x) {
                if(x == std::stoi(message)){
                    auto finalRes = std::string();
                    for(int i = 0; i < res.size(); i+=8){
                        std::string sub = res.substr(i, 8);
                        if(sub.size() == 8){
                            std::bitset<8> bitSubset(sub);
                            char c = static_cast<char>(bitSubset.to_ulong());
                            finalRes += c;
                        }
                    }
                    std::cout << "Decrypted message: " << finalRes << std::endl;
                    return;
                }

                /// Calculating index to iterate through image(pixel) data
                int index = (x + y * fileInfoHeader.width) * 3;
                if(pixelData.empty()){
                    std::cerr << "Error! Image(pixel) data is empty!" << std::endl;
                    return;
                }

                /// Checking index + 2 not to be bigger or equal than image(pixel) data size
                if(index + 2 >= pixelData.size()){
                    std::cerr << "Error!" << std::endl;
                    continue;
                }

                /// Receiving RGB values
                unsigned char blue = pixelData[index];
                unsigned char green = pixelData[index + 1];
                unsigned char red = pixelData[index + 2];

                /// Converting RGB values to bitsets
                std::bitset<8> red_value(red);
                std::bitset<8> green_value(green);
                std::bitset<8> blue_value(blue);

                /// Lambda function to get 2 Least Significant Bits
                auto getBit = [&res](std::bitset<8>& bitset){
                    res += std::to_string(bitset[1]) += std::to_string(bitset[0]);
                };

                getBit(red_value);
                getBit(green_value);
                getBit(blue_value);
            }
        }
    }

    /**
     * @brief Check whether given message can be written into a file(image)
     * @function check
     *
     * @param path -> path of the file(image)
     * @param msg -> provided message
     *
     *
     */

    auto check(const std::string& path, const std::string& msg) -> void{
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }

        BMP_FileHeader fileHeader;              // <<File Header[fileType, fileSize, dataOffset]>>
        BMP_FileInfoHeader fileInfoHeader;      // <<File Header Information[size, width, height, bitCount, compression, ...]>>

        /// Opening file [binary mode]
        std::ifstream file(path,std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Unable to open file! Path provided: " << path << std::endl;
            return;
        }
        /// Reading file
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        file.read(reinterpret_cast<char*>(&fileInfoHeader), sizeof(fileHeader));

        /// Printing information
        std::cout << "Message \"" << msg << "\" size is " << msg.size() << " bytes." << std::endl;
        std::cout << "Size file can store - " << fileHeader.fileSize << " bytes." << std::endl;

        if((msg.size() > fileHeader.fileSize)){
            std::cerr << "Size of message is bigger than size file can store!" << std::endl;
            return;
        }
        else std::cout << "Size of message is acceptable for file(image)" << std::endl;

        /// Closing file
        file.close();
    }
}

namespace ppm{
    /**
    * @brief Get detailed information about the file(image).
    * @function info
    *
    * @param path -> path of the file(image)<br>
    * @flags -i <i>OR</i> --info
    * @details This function is used to get varity of details about the specified file(image), such as:<br>
    *              &emsp;&emsp;- Magic Number<br>
    *              &emsp;&emsp;- Image Width<br>
    *              &emsp;&emsp;- Image Height<br>
    *              &emsp;&emsp;- Maximum Color Value<br>
    *              &emsp;&emsp;- Image(pixel) data
    * */

    auto info(const std::string& path)->void{
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }

        PPM_FileHeader imageHeader;

        /// Opening file in binary mode
        std::ifstream ppm_file(path);
        if (!ppm_file) {
            std::cerr << "Unable to open file! Path provided: " << path << std::endl;
            return;
        }

        /// Reading file information [magic number, width, height, max color value]
        ppm_file >> imageHeader.magic_number >> imageHeader.width >> imageHeader.height
                 >> imageHeader.max_color_val;

        /// Printing received information
        std::cout << "Magic Number(Type): " << imageHeader.magic_number << std::endl;
        std::cout << "Width: " << imageHeader.width << std::endl;
        std::cout << "Height: " << imageHeader.height << std::endl;
        std::cout << "Max color value: " << imageHeader.max_color_val << std::endl;

        /// Closing File
        ppm_file.close();
    }

    /**
    * @brief Encrypt message into image
    * @function encrypt
    *
    * @param path -> path of the file(image)
    * @param msg -> message that should be encrypted
    * @flags -e <i>OR</i> -encrypt
    * @details This function is used to encrypt provided message into the image
    * */

    auto encrypt(const std::string &path, std::string msg) {
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }

        PPM_FileHeader imageHeader;

        /// Converting message into bit representation(long_string)
        std::bitset<8> string_to_b[100];
        for (int i = 0; i < msg.size(); ++i) string_to_b[i] = std::bitset<8>(msg[i]);
        std::string long_string;
        for (int i = 0; i < msg.size(); ++i) {
            for (int j = 7; j >= 0; --j)
                long_string += std::to_string(string_to_b[i][j]);
        }

        /// Reading file
        if (!ppm::readPPMImage(path, imageHeader)) {
            std::cout << "Error while reading file! Path provided: " << path << std::endl;
            return;
        }



        auto res = std::string();
        for (int y = 0; y < imageHeader.height; y++) {
            for (int x = 0; x < imageHeader.width; ++x) {
                /// Checking if long_string is empty (end of the looping, message is encrypted)
                if (long_string.empty()) {
                    ppm::writeToPPM(path, imageHeader);
                    std::ofstream message_log("..\\ImageStegonography\\message_log.txt",
                                              std::ios::app);
                    message_log << x;
                    message_log.close();
                    std::cout << "Message is successfully encrypted into ppm_encrypted_file.ppm!" << std::endl;
                    return;
                }

                /// Calculating index to access image(pixel) data and extracting RGB values from each pixel
                int index = (x + y * imageHeader.width) * 3;
                unsigned char red = imageHeader.image_data[index];
                unsigned char green = imageHeader.image_data[index + 1];
                unsigned char blue = imageHeader.image_data[index + 2];

                /// Converting RGB values into bitsets
                std::bitset<8> red_value(red);
                std::bitset<8> green_value((char) green);
                std::bitset<8> blue_value((char) blue);

                /// Lambda function to change bit at some index in bitsets (bit representaion of RGB values)
                auto change_value = [&long_string](std::bitset<8> &bitset, int index) -> void {
                    if (index < 0 || index >= bitset.size()) {
                        std::cerr << "Error! Index: " << index << " is out of bounds for bitset of size: "
                                  << bitset.size() << std::endl;
                        return;
                    }
                    if (long_string.empty()) {
                        return;
                    }
                    if (long_string[0] == '0') bitset.set(index, 0);
                    else bitset.set(index, 1);
                    long_string.erase(0, 1);
                };


                change_value(red_value, 0);
                change_value(green_value, 0); // 0 - LSB, 1 - second LSB
                change_value(blue_value, 0);

                /// Changing each RGB values in image(pixel) data
                imageHeader.image_data[index] = static_cast<unsigned char>(red_value.to_ulong());
                imageHeader.image_data[index + 1] = static_cast<unsigned char>(green_value.to_ulong());
                imageHeader.image_data[index + 2] = static_cast<unsigned char>(blue_value.to_ulong());
            }
        }
    }

    /**
    * @brief Decrypt message from image
    * @function decrypt
    *
    * @param path -> path of the file(image)
    * @flags -d <i>OR</i> -decrypt
    * @details This function is used to decrypt message from the image
    * @attention * std::bitset<8> stores LSB on the right side, so for example (7,6,5,4,3,2,1,0) bitset will have a LSB at position 0<br>
    * */


    auto decrypt(const std::string &path) {
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }

        PPM_FileHeader imageHeader;

        /// Getting size of long_string
        std::string message, line;
        std::ifstream message_log("..\\ImageStegonography\\message_log.txt");
        while (std::getline(message_log, line)) message += line;
        message_log.close();
        /// Deleting the content of the message log file
        std::ofstream mf("..\\ImageStegonography\\message_log.txt", std::ios::out | std::ios::trunc);
        mf.close();

        /// Opening file
        std::ifstream file(path,
                           std::ios::binary);

        /// Reading File
        file >> imageHeader.magic_number >> imageHeader.width >> imageHeader.height >> imageHeader.max_color_val;
        file.ignore();

        int numberOfPixels = imageHeader.width * imageHeader.height;
        imageHeader.image_data.resize(numberOfPixels * 3);
        file.read(reinterpret_cast<char*>(imageHeader.image_data.data()), imageHeader.image_data.size());
        if(!file){
            std::cout << "Not a pixel data!" << std::endl;
            return;
        }
        if (!file.is_open()) {
            std::cerr << "Failed to open file." << std::endl;
            return; //the program does not execute successfully and there is some error
        }
        file.close();

        auto res = std::string();
        for (int y = 0; y < imageHeader.height; y++) {
            for (int x = 0; x < imageHeader.width; ++x) {
                if (x == std::stoi(message)) {
                    auto finalRes = std::string();
                    for (int i = 0; i < res.size(); i += 8) {
                        std::string sub = res.substr(i, 8);
                        if (sub.size() == 8) {
                            std::bitset<8> bitSubset(sub);
                            char c = static_cast<char>(bitSubset.to_ulong());
                            finalRes += c;
                        }
                    }
                    std::cout << "Decrypted message: " << finalRes << std::endl;
                    return;
                }
                /// Calculating index
                int index = (x + y * imageHeader.width) * 3;
                if (imageHeader.image_data.empty()) {
                    std::cerr << "Error! Image(pixel) data is empty!" << std::endl;
                    return;
                }

                if (index + 2 >= imageHeader.image_data.size()) {
                    std::cerr << "Error!" << std::endl;
                    continue;
                }

                /// Receiving RGB values
                unsigned char red = imageHeader.image_data[index];
                unsigned char green = imageHeader.image_data[index + 1];
                unsigned char blue = imageHeader.image_data[index + 2];

                /// Converting RGB values to bitsets
                std::bitset<8> red_value(red);
                std::bitset<8> green_value(green);
                std::bitset<8> blue_value(blue);

                /// Storing LSB in res variable
                res += std::to_string(red_value[0]);
                res += std::to_string(green_value[0]);
                res += std::to_string(blue_value[0]);
            }
        }
    }

    /**
     * @brief Check whether given message can be written into a file(image)
     * @function check
     *
     * @param path -> path of the file(image)
     * @param msg -> provided message
     *
     *
     */

    auto check(const std::string& path, const std::string& msg){
        /// Checking whether path was provided and its correctness
        if(path.empty()){
            std::cerr << "Path is incorrect or was not provided!" << std::endl;
            return;
        }
        PPM_FileHeader ppm;
        /// Opening file
        std::ifstream ppm_file(path,std::ios::binary);
        if (!ppm_file.is_open()) {
            std::cerr << "Unable to open file! Path provided: " << path << std::endl;
            return;
        }

        /// Reading magic number(type)
        ppm_file >> ppm.magic_number;

        /// Checking magic number(type) [P3 - plain text, P6 - binary]
        if (ppm.magic_number != "P6") {
            std::cerr << "Unsupported BMP format." << std::endl;
            ppm_file.close();
            return;
        }

        /// Reading other data
        ppm_file >> ppm.width >> ppm.height >> ppm.max_color_val;
        /// Skipping whitespace
        ppm_file.ignore();

        /// Calculating number of pixels and resizing image(pixel) data to this size
        int numberOfPixels = ppm.width * ppm.height;
        int availableBits = numberOfPixels * 24; // each pixel occupies 24 bits(3 bytes)
        ppm.image_data.resize(numberOfPixels * 3);

        /// Reading pixel data from the file(image)
        ppm_file.read(reinterpret_cast<char *>(&ppm.image_data[0]), ppm.image_data.size());
        if (!ppm_file) {
            std::cout << "Error while reading image(pixel) data!" << std::endl;
            return;
        }

        /// Printing Received Information
        std::cout << "Message \"" << msg << "\" size is " << msg.size() << " bytes." << std::endl;
        std::cout << "Size file can store - " << (availableBits / 8) << " bytes." << std::endl;
        if((msg.size() > (availableBits / 8))) {
            std::cerr << "Size of the message is bigger than size file can store!" << std::endl;
            return;
        }
        std::cout << "Message can be encrypted into the file(image)" << std::endl;

        /// Closing file
        ppm_file.close();
    }

}


/**
 *  @function help
 *  @flags -h || --help
 *  @details Function is used to show 'help' information for users
*/

auto help() -> void
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





/**
     * @SourceOfInformation
     * <h3>ChatGPT(OpenAI):</h3>
     * <h4>1) How to calculate the index of RGB values in image(pixel) data?</h4>
     * &emsp; Me: Tell me about .bmp file type and what main properties it has<br><br>
     *
     * &emsp; ChatGPT: To calculate the index of pixels in the image data of a .bmp file,
     *                 you need to understand the structure of the BMP file format. Here’s
     *                 a basic overview of how pixel indexing works in a typical BMP file:
     *                 &emsp;1.<b>BMP File Header: </b>The BMP file starts with a header that contains
     *                 information about the file, such as file type, size, offset to the pixel data, etc.<br>
     *                 &emsp;2.<b>Image Data Offset: </b>Inside the BMP file header, there is an offset value
     *                                                   that specifies the starting address of the pixel data.
     *                                                   This offset tells you where the actual pixel data
     *                                                   begins in the file.<br>
     *                 &emsp;3.<b>Pixel Data Format: </b>The pixel data in a BMP file is typically organized
     *                                                   row by row from bottom to top (when the image is displayed).
     *                                                   Each row is aligned to a multiple of 4 bytes
     *                                                   (sometimes called padding).<br>
     *                 &emsp;4.<b>Calculation of Pixel Index: </b>- Let’s assume you want to find the index of a pixel at coordinates (x, y) in the image.<br>
     *                                                            - Determine the size of each pixel based on the image bit depth (e.g., 24 bits per pixel means 3 bytes per pixel).<br>
     *                                                            - Calculate the width of each row in bytes (including padding) based on the image width and bit depth.<br>
     *                                                            - Use the formula to calculate the index:<br>
     *                                                            &emsp;&emsp;index=offset+(height−y−1)×row_size+x×bytes_per_pixel<br>
     *                                                            &emsp;- offset is the offset to the pixel data from the start of the file.<br>
     *                                                            &emsp;- height is the height of the image in pixels.<br>
     *                                                            &emsp;- y is the row index of the pixel (0 being the top row).<br>
     *                                                            &emsp;- row_size is the size of each row in bytes, accounting for padding.<br>
     *                                                            &emsp;- x is the column index of the pixel.<br>
     *                                                            &emsp;- bytes_per_pixel is the number of bytes per pixel (typically 3 for 24-bit BMP).<br>
     * <br><h4>2) Position of RGB values in .bmp and .ppm files</h4>
     * &emsp; Me: What is the position of RGB values in image(pixel) data in .bmp files and .ppm files? Shortly<br><br>
     *
     * &emsp; ChatGPT: In both BMP (Bitmap) and PPM (Portable Pixel Map) file formats:
     *                 &emsp;1.<b>BMP (Bitmap): </b>RGB values are typically stored in a specific order
     *                                             within the pixel data of a BMP file. The order is Blue (B),
     *                                             Green (G), Red (R). This sequence is sometimes referred to
     *                                             as BGR format.<br>
     *                 &emsp;2.<b>PPM (Portable Pixel Map): </b>PPM files store RGB values in a different order
     *                                                          compared to BMP files. In PPM format, the order
     *                                                          of RGB values within each pixel is Red (R),
     *                                                          Green (G), Blue (B).<br>
     *                 So, the key difference is the order in which RGB values are stored:
     *                 &emsp;&emsp;- BMP: BGR (Blue-Green-Red)
     *                 &emsp;&emsp;- PPM: RGB (Red-Green-Blue)
     *                 This difference is important when reading or manipulating pixel data directly from these file formats.<br>
     */

