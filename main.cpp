#include <iostream>
#include <regex>

#include "MainFunctions.h"

int main(int argc, char* argv[]) {
    std::regex bmp_pattern(".*\\.bmp$"), ppm_pattern(".*\\.ppm$");
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == "-i" || arg == "-info" && i + 1 < argc){
            std::string path = argv[++i];
            if(std::regex_match(path, bmp_pattern)) bmp::info(path);
            else if(std::regex_match(path, ppm_pattern)) ppm::info(path);
            else std::cerr << "Incorrect file type provided! Try using -h OR -help flag to get help information." << std::endl;
            return 0;
        }else if(arg == "-e" || arg == "-encrypt" && i + 2 < argc){
            std::string path = argv[++i];
            std::string msg = argv[++i];
            if(std::regex_match(path, bmp_pattern)) bmp::encrypt(path, msg);
            else if(std::regex_match(path, ppm_pattern)) ppm::encrypt(path, msg);
            else std::cerr << "Incorrect file type provided! Try using -h OR -help flag to get help information." << std::endl;
            return 0;
        }else if(arg == "-d" || arg == "-decrypt" && i + 1 < argc){
            std::string path = argv[++i];
            if(std::regex_match(path, bmp_pattern)) bmp::decrypt(path);
            else if(std::regex_match(path, ppm_pattern)) ppm::decrypt(path);
            else std::cerr << "Incorrect file type provided! Try using -h OR -help flag to get help information." << std::endl;
            return 0;
        }else if(arg == "-c" || arg == "-check" && i + 2 < argc){
            std::string path = argv[++i];
            std::string msg = argv[++i];
            if(std::regex_match(path, bmp_pattern)) bmp::check(path, msg);
            else if(std::regex_match(path, ppm_pattern)) ppm::check(path, msg);
            else std::cerr << "Incorrect file type provided! Try using -h OR -help flag to get help information." << std::endl;
            return 0;
        }else if(arg == "-h" || arg == "-help"){
            help();
            return 0;
        }
    }
    std::cerr << "Error! Incorrect flag used! Try using -h OR -help flag to get help information" << std::endl;

    return 0;
}