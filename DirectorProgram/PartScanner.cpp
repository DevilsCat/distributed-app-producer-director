// PartScanner.cpp -- Implements a scanner to scan line from Part file.
// Create by Yu Xiao
//
#include "stdafx.h"
#include "PartScanner.h"
#include "utils.h"

PartScanner::PartScanner(std::ifstream& ifs) :
    Scanner(ifs), CONFIG_RE("^(.+) (.+\\.txt)$")
{
    if (ifs_.is_open() && !ifs_.eof()) {
        *this >> ptr_;
    }
}

bool PartScanner::operator>>(std::shared_ptr<NodeAST>& container_ptr) {
    // Get one line from file
    std::string line;

    do { // try to skip the empty line
        if (ifs_.eof()) break;
        std::getline(ifs_, line);
    } while (line.empty());

    if (line.empty()) { return false; } // file end and no more line available

    // Match the line
    std::smatch match;

    if (std::regex_search(line, match, CONFIG_RE) && match.size() == 3) {
        std::string character_name = match.str(1);
        container_ptr = std::make_shared<PartAST>(character_name, std::string(path_string + match.str(2)));
    }
    else {
        container_ptr = std::make_shared<UnknownNodeAST>();
    }

    return true;
}