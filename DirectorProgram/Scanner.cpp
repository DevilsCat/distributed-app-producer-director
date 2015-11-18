// Scanner.h -- Implements general Scanner, which contains most common operation for sub Scanner.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#include "stdafx.h"
#include "Scanner.h"

Scanner::Scanner(std::ifstream& ifs) :
    ifs_(ifs)
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
