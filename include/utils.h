#pragma once
#include <string>

inline bool ends_with(const std::string& s, const std::string& suffix) {
    return s.size() >= suffix.size() && s.rfind(suffix) == s.size() - suffix.size();
}

void parseArgs(int argc, char* argv[]);