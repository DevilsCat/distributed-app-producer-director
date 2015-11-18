// InterruptFlag.cpp -- Implements a thread local interruption flag used for interrupt thread.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#include "stdafx.h"
#include "InterruptFlag.h"
#include "ProgramException.h"

__declspec(thread) bool ThreadInterruptFlag;

void InterruptionPoint() {
    if (ThreadInterruptFlag) {
        throw ProgramException("Woker Thread Has Been Interrupted.", ProgramException::THREAD_BEEN_INTERRUPTED);
    }
}