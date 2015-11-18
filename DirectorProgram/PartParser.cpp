// PartParser.h -- Implements a parser for parsing Part file
// Created by Yu Xiao
//
#include "stdafx.h"
#include "PartParser.h"
#include "ProgramException.h"

PartParser::PartParser(std::ifstream& ifs, const std::string& path) :
    ifs_(move(ifs)), path_(path), part_scanner_(ifs_, path_)
{
    if (!ifs_.is_open()) {
        throw ProgramException("part file open failed.", ProgramException::ExceptionType(errno));
    }
}

void PartParser::Parse() {
    if (!part_scanner_) {
        throw ProgramException("Part File is Empty.", ProgramException::EMPTY_FILE_EXCEPTION);
    }

    while (std::shared_ptr<PartAST> part_ptr = ParsePart()) {
        part_ptrs_.push_back(part_ptr);
    }

    it_ = part_ptrs_.begin();

    ifs_.close();
}

bool PartParser::HasNext() const {
    return it_ != part_ptrs_.end();
}

std::vector<std::shared_ptr<PartAST>>::iterator& PartParser::GetIterator() {
    it_ = part_ptrs_.begin();
    return it_;
}

std::shared_ptr<PartAST> PartParser::ParsePart() {
    if (part_scanner_.peek() != NodeAST::PART) {
        return std::shared_ptr<PartAST>();
    }

    std::shared_ptr<PartAST> part_ptr = std::dynamic_pointer_cast<PartAST>(part_scanner_.pop());

    return part_ptr;
}
