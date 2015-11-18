// ScriptScanner.cpp -- Implements a scanner to scan line from Script file.
// Create by Yu Xiao
//
#include "stdafx.h"
#include "ScriptScanner.h"

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