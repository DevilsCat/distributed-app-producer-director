// Scanner.cpp -- Implements general Scanner, which contains most common operation for sub Scanner.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#include "stdafx.h"
#include "Scanners.h"

#define PART_REGMATCH_NUM   3
#define PART_CHAR_NAME_POS       1

Scanner::Scanner(std::ifstream& ifs, std::string path) :
    path_(path), ifs_(ifs)
{}

Scanner::~Scanner() {}

Scanner::operator bool() const {
    return ptr_ != nullptr;
}

NodeAST::NodeType Scanner::peek() const {
    return ptr_ ? ptr_->type_ : NodeAST::UNKNOWN; // ptr_ might be empty.
}

std::shared_ptr<NodeAST> Scanner::pop() {
    std::shared_ptr<NodeAST> res = ptr_;
    if (!(*this >> ptr_)) {
        ptr_ = std::shared_ptr<NodeAST>();
    }
    return res;
}

// class ScriptScanner
ScriptScanner::ScriptScanner(std::ifstream& ifs) :
    Scanner(ifs),
    SCENE_RE("\\[scene\\] (.*)"),
    FRAG_RE("(.+)\\.txt")
{
    if (ifs_.is_open() && !ifs_.eof()) { // store the first container pointer.
        *this >> ptr_;
    }
}

bool ScriptScanner::operator>> (std::shared_ptr<NodeAST>& container_ptr) {
    // Get one line from file
    std::string line;
    do { // try to skip the empty line
        if (ifs_.eof()) break;
        std::getline(ifs_, line);
    } while (line.empty());

    if (line.empty()) { return false; } // file end and no more line available

    // Match the line 
    std::smatch match;

    if (std::regex_search(line, match, SCENE_RE) && match.size() > 1) { // match scene
        std::string scene_title = match.str(1);
        container_ptr = std::make_shared<SceneAST>(scene_title);
    }
    else if (std::regex_match(line, FRAG_RE)) { // match fragment
        std::string frag_conf_file = line;
        container_ptr = std::make_shared<FragmentAST>(frag_conf_file);
    }
    else { // default
        container_ptr = std::make_shared<UnknownNodeAST>();
    }

    return true;
}

// PartScanner
PartScanner::PartScanner(std::ifstream& ifs, std::string& path) :
    Scanner(ifs, path), CONFIG_RE("^(.+) (.+\\.txt)$")
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

    if (regex_search(line, match, CONFIG_RE) && match.size() == PART_REGMATCH_NUM) {
        std::string character_name = match.str(PART_CHAR_NAME_POS);
        container_ptr = std::make_shared<PartAST>(character_name, std::string(path_ + match.str(2)));
    }
    else {
        container_ptr = std::make_shared<UnknownNodeAST>();
    }

    return true;
}
