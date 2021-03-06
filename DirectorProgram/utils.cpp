// utility function for this lab

#include "stdafx.h"
#include "utils.h"
#include <regex>
#include <sstream>
#include <iterator>
#include <ostream>

std::string path_string;

std::string GetFilePath(const std::string& str) {
    const std::string tmp_path = str.substr(0, str.find_last_of("\\/")); // works both well for "\\" and "/"
    if (!tmp_path.compare(str)) { return std::string(); }
    // if we simply pass the filename, then tmp_path will contains the same string as given argument, simply ignore
    // otherwise, do the following
    std::string slash_string("\\");
    return tmp_path + slash_string;
}

std::string GetFileName(const std::string& str) {
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath_s(str.c_str(), drive, dir, fname, ext);
    return std::string(fname);
}

std::string pad_whitespace(unsigned n) {
	std::string s;
	for (unsigned i = 0; i < 2 * n; ++i) {
		s += " ";
	}
	return s;
}

unsigned to_number(char* num_str) {
    return to_number(std::string(num_str));
}

unsigned to_number(const std::string& num_str) {
    return atoi(num_str.c_str());
}

bool is_number(char* s) {
    return is_number(std::string(s));
}

bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !isdigit(c); }) == s.end();
}

bool is_ip_addr(char* s) {
    std::regex reg("\\d{1,3}(\\.\\d{1,3}){3}|localhost");
    return regex_match(std::string(s), reg);
}

bool is_filename(char* s) {
    std::regex reg(".+\\.txt");
    return regex_match(std::string(s), reg);
}

std::vector<std::string> tokenize(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         back_inserter(tokens));
    return tokens;
}
