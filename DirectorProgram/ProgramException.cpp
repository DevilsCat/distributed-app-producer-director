// ProgramException.cpp -- Implements the exception that will be thrown in this program and assigned different error code
// for debugging.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#include "stdafx.h"
#include "ProgramException.h"
#include "sstream"

ProgramException::ProgramException(const std::string& what, const ExceptionType type) :
std::runtime_error(what), type_(type)
{}

int ProgramException::GetExceptionCode() {
    return type_;
}

const char* ProgramException::what() const {
    std::ostringstream oss("");
    oss << runtime_error::what() << "Error Code: " << type_;
    std::string str = oss.str();
    return _strdup(str.c_str());
}
