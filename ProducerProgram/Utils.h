#ifndef UTILS_H 
#define UTILS_H
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace utils {
    inline 
    std::vector<std::string> tokenize(const std::string& line) {
        std::istringstream iss(line);
        return std::vector<std::string> {
            std::istream_iterator<std::string> {iss},
            std::istream_iterator<std::string> {}
        };
    }

    inline
    unsigned to_number(const std::string& num_str) {
        return atoi(num_str.c_str());
    }

    inline
    bool is_number(const std::string& s) {
        return !s.empty() && std::find_if(s.begin(),
            s.end(), [](char c) { return !isdigit(c); }) == s.end();
    }
}

#endif
