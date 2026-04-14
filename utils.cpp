#include <iostream>
#include "utils.h"
#include "MainFunctions.h"

const std::string INCORRECT_FILE_TYPE_MSG = "Incorrect file type provided! Try using -h OR -help flag to get help information.";

void parseArgs(int argc, char* argv[]) {
    if (argc < 2) { help(); return; }

    std::string flag = argv[1];

    if (flag == "-h" || flag == "-help") {
        help();
    } else if((flag == "-i" || flag == "-info") && argc >= 3) {
        std::string path = argv[2];
        if(ends_with(path, ".bmp")) bmp::info(path);
        else if(ends_with(path, ".ppm")) ppm::info(path);
        else std::cerr << INCORRECT_FILE_TYPE_MSG << '\n';
    } else if((flag == "-e" || flag == "-encrypt") && argc >= 4){
        std::string path = argv[2], msg = argv[3];
        if(ends_with(path, ".bmp")) bmp::encrypt(path, msg);
        else if(ends_with(path, ".ppm")) ppm::encrypt(path, msg);
        else std::cerr << INCORRECT_FILE_TYPE_MSG << '\n';
    } else if((flag == "-d" || flag == "-decrypt") && argc >= 3){
        std::string path = argv[2];
        if(ends_with(path, ".bmp")) bmp::decrypt(path);
        else if(ends_with(path, ".ppm")) ppm::decrypt(path);
        else std::cerr << INCORRECT_FILE_TYPE_MSG << '\n';
    } else if((flag == "-c" || flag == "-check") && argc >= 4){
        std::string path = argv[2], msg = argv[3];
        if(ends_with(path, ".bmp")) bmp::check(path, msg);
        else if(ends_with(path, ".ppm")) ppm::check(path, msg);
        else std::cerr << INCORRECT_FILE_TYPE_MSG << '\n';
    }
}

