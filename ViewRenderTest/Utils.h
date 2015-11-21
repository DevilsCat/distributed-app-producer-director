#pragma once
#include <sstream>
#include <string>

namespace utils {
    namespace windows {
        inline 
        std::string center(const std::string& s, const int& w, const char delimiter) {
            std::stringstream ss, spaces;
            int padding = w - s.size() - 1;                 // count excess room to pad
            for (int i = 0; i<padding / 2; ++i)
                spaces << delimiter;
            ss << spaces.str() << s << spaces.str();    // format with padding
            if (padding>0 && padding % 2 != 0)               // if odd #, add 1 space
                ss << delimiter;
            return ss.str();
        }

        inline
        std::string left(const std::string s, const int w, const char delimiter) {
            std::stringstream ss, spaces;
            int padding = w - s.size() - 1;                 // count excess room to pad
            for (int i = 0; i<padding; ++i)
                spaces << delimiter;
            ss << s << spaces.str();                    // format with padding
            return ss.str();
        }
    }
}
