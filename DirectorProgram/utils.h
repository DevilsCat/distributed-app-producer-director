// utils.h -- Declare utililty function which will be used in the program, i.e. file operation.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao
//
#ifndef UTILS_H
#define UTILS_H

#include "stdafx.h"
#include <fstream>
#include <string>
#include <vector>

#define DEBUG   1

#define DEBUG_PRINTF(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

//
// GetFilePath()
std::string GetFilePath(const std::string& str);
std::string GetFileName(const std::string& str);
//
// ParseFile()
// this template function parse the file given filename, using user-provided function. non-negative/non-zero integer
// will return if open file failed (uses errno to indicate all possible kind of failure). -1 will be returned, if 
// user-provided functions failed to read/parse the content from file. Returns 0 means success.
//
template<typename Container>
int ParseFile(std::ifstream& in, Container& c, void(*func) (unsigned int idx, std::string&, Container&)) {
	if (!in.is_open()) {
		int error_code = errno;
		//std::cerr << "open file failed" << std::endl;
		return error_code;
	}
	// otherwise, open succeed
	unsigned int idx = 0;
	for (std::string line; getline(in, line); ++idx) {
		func(idx, line, c);
	}
	// check container not to be empty
	if (c.empty()) { 
		//std::cout << "read file failed" << std::endl;
		return -1;
	} 

	return 0;
}

std::string pad_whitespace(unsigned int n);

unsigned to_number(char* num_str);

unsigned to_number(const std::string& num_str);

bool is_number(char* s);

bool is_number(const std::string& s);

bool is_ip_addr(char* s);

bool is_filename(char* s);

std::vector<std::string> tokenize(const std::string& line);
#endif
