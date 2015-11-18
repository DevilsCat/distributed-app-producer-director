// InterrrupibleThread.h -- Declares and implements an interruptible thread here.
// Created by Yuhan Hao, Yu Xiao, Anqi Zhang.
//
#ifndef INTERRUPTIBLE_THREAD_H
#define INTERRUPTIBLE_THREAD_H
#include <thread>
#include <future>
#include <iostream>
#include "InterruptFlag.h"
#include "ProgramException.h"

class InterruptibleThread {
    typedef bool* FlagType;
public:
    template<typename FuncType>
    InterruptibleThread(FuncType f) {
        std::promise<FlagType> p;
        internalThread = std::thread([&]{
            p.set_value(&ThreadInterruptFlag);
            try {
                f();
            } catch (ProgramException& e) {
                std::cerr << "[" << std::this_thread::get_id() << "]" << e.what() << std::endl;
            }
        });
        interruptFlag = p.get_future().get();
    }

    ~InterruptibleThread() {
        if (internalThread.joinable()) {
            internalThread.join();
        }
    }

    void detach() {
        internalThread.detach();
    }

    void interrupt() {
        if (interruptFlag) {
            *interruptFlag = true;
        }
    }

private:
    std::thread internalThread;
    bool *interruptFlag;

};

#endif