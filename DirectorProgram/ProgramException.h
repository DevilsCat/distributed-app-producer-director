// ProgramException.h -- Declares the exception that will be thrown in this program and assigned different error code
// for debugging.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#ifndef PROGRAM_EXCEPTION
#define PROGRAM_EXCEPTION
#include <vector>

class ProgramException : public std::runtime_error {
public:
    enum ExceptionType {  // User-defined error code always be negative.
        EMPTY_FILE_EXCEPTION = -1,
        SCENE_FRAGMENT_INVALID = -2,
        ON_STAGE_COUNTER_INVALID = -3,
        THREAD_BEEN_INTERRUPTED = -4
    };

	//initialize with the description and the exception type
    ProgramException(const std::string& what, const ExceptionType type);

	//get getExceptionCode
    int GetExceptionCode();

    virtual const char* what() const override;

private:
    ExceptionType type_;
};

#endif
