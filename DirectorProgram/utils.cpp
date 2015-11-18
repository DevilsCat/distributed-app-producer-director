// utility function for this lab

#include "stdafx.h"
#include "utils.h"

std::string path_string;
bool override_flag;

void SetPathFromArg(std::string const& arg) {
	const std::string tmp_path = arg.substr(0, arg.find_last_of("\\/")); // works both well for "\\" and "/"
	if (!tmp_path.compare(arg)) { return; }
	// if we simply pass the filename, then tmp_path will contains the same string as given argument, simply ignore
	// otherwise, do the following
	std::string slash_string("\\");
	path_string = tmp_path + slash_string;
}

void SetOverrideFlagFromArg(std::string const& arg) {
    override_flag = arg == "-override";
}

std::string pad_whitespace(unsigned n) {
	std::string s;
	for (unsigned i = 0; i < 2 * n; ++i) {
		s += " ";
	}
	return s;
}

