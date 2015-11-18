// ScriptParser.cpp -- Declares a parser for parsing Script file
// Created by Yu Xiao
//
#include "stdafx.h"
#include "ScriptParser.h"
#include "PartParser.h"
#include "utils.h"
#include "PrintVisitor.h"
#include "ProgramException.h"

ScriptParser::ScriptParser(std::string& s) :
    filename_(s), ifs_(filename_), scanner_(ifs_)
{
    // check open failed
    if (!ifs_.is_open()) {
        throw ProgramException("script file open failed.", ProgramException::ExceptionType(errno));
    }
}

void ScriptParser::Parse() {
    script_ptr_ = ParseScript();
    ifs_.close();
}

std::shared_ptr<ScriptAST> ScriptParser::script_ptr() {
    return script_ptr_;
}

void ScriptParser::PrintScript() {
    PrintVisitor p;
    script_ptr_->accept(p);
}

std::shared_ptr<ScriptAST> ScriptParser::ParseScript() {

    std::shared_ptr<ScriptAST> res = std::make_shared<ScriptAST>(ScriptAST(filename_));

    while (scanner_) {
        res->add_node(ParseScene());
    }

    return res;
}

std::shared_ptr<SceneAST> ScriptParser::ParseScene() {
    if (scanner_.peek() != NodeAST::SCENE) {
        throw std::runtime_error("Unknown scene struct.");
    }

    std::shared_ptr<SceneAST> scene_ptr = std::dynamic_pointer_cast<SceneAST>(scanner_.pop());

    while (std::shared_ptr<FragmentAST> frag_ptr = ParseFragement()) {
        scene_ptr->add_node(frag_ptr);
    }

    return scene_ptr;
}

std::shared_ptr<FragmentAST> ScriptParser::ParseFragement() {
    if (scanner_.peek() != NodeAST::FRAGEMENT) { // no fragment detected
        return std::shared_ptr<FragmentAST>();
    }

    std::shared_ptr<FragmentAST> frag_ptr = std::dynamic_pointer_cast<FragmentAST>(scanner_.pop());

    std::ifstream part_ifs(path_string + frag_ptr->string_);

    PartParser p(part_ifs);

    p.Parse(); // parse the config file

    auto &it = p.GetIterator(); // ATTENTION: reference forces!

    while (p.HasNext()) {
        frag_ptr->add_node(*it);
        ++it;
    }

    return frag_ptr;
}
