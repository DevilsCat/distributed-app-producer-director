#ifndef UTILS_H 
#define UTILS_H
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <iostream>

#ifndef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#endif

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

        inline
        std::string truncate(const std::string& s, size_t w, bool show_ellipsis = true) {
            const std::string sEllipsis = "...";
            if (s.length() > w)
                return show_ellipsis ? s.substr(0, w - sEllipsis.size()) + sEllipsis : s.substr(0, w);
            return s;
        }

        inline
        void GoToXY(const short& x, const short& y) {
            COORD homeCoords = { x, y };
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), homeCoords);
        }

        inline
        int GetCursorPos(short& x, short& y) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            int ret = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            if (ret) {
                x = csbi.dwCursorPosition.X;
                y = csbi.dwCursorPosition.Y;
            }
            return ret;
        }

        inline
        std::string StringPrintf(const char* format, ...) {
            va_list arg_list;
            va_start(arg_list, format);

            char short_buf[256];
            const size_t needed = vsnprintf_s(short_buf, sizeof short_buf,
                format, arg_list) + 1;
            if (needed <= sizeof short_buf)
                return short_buf;

            return std::string();
        }
    }
}

#endif
