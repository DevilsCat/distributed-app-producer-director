// utils.h -- Declare utililty function which will be used in the program, i.e. file operation.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao
//
#ifndef UTILS_H
#define UTILS_H

#include "stdafx.h"
#include <fstream>
#include <string>

// Stores where the input file is (derived from configure file)
extern std::string path_string;

// Stores if the user set it to override.
extern bool override_flag;

//
// SetPathFromArg()
// This function set the path for all inputfiles given the input configure file path
// and store the path into global variable path_string.
//
void SetPathFromArg(std::string const &arg);

//
// SetOverrideFlagFromArg()
// This function sets the override flag given the input arguments, and store into
// a global flag.
//
void SetOverrideFlagFromArg(std::string const &arg);

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
#endif