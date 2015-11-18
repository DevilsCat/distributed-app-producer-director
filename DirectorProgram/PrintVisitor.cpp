// PrinterVisitor.h -- Implements a concrete printing operation on visiting AST.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao
//
#include "stdafx.h"
#include "utils.h"
#include "PrintVisitor.h"
#include "ASTNodes.h"
#include <iostream>
#include <string>

void PrintVisitor::Visit(ScriptAST* node) {
    std::cout << pad_whitespace(0) << "[script] " << node->string_ << std::endl;
}

void PrintVisitor::Visit(SceneAST* node) {
    std::cout << pad_whitespace(1) << "[scene] " << node->string_ << std::endl;
}

void PrintVisitor::Visit(FragmentAST* node) {
    std::cout << pad_whitespace(2) << "[frag] " << node->string_ << std::endl;
}

void PrintVisitor::Visit(PartAST* node) {
    std::cout << pad_whitespace(3) << "[part] " << node->string_ << " : " << node->dialog_filename_ << std::endl;
}

void PrintVisitor::Visit(UnknownNodeAST* node) {}