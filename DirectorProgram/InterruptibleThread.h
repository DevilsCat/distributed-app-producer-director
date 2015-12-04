// InterrrupibleThread.h -- Declares and implements an interruptible thread here.
// Created by Yuhan Hao, Yu Xiao, Anqi Zhang.
//
#ifndef INTERRUPTIBLE_THREAD_H
#define INTERRUPTIBLE_THREAD_H
#include <thread>
#include <future>
#include "InterruptFlag.h"
#include "ProgramException.h"
#include "utils.h"

class InterruptibleThread {
    typedef bool* FlagType;
public:

	//Constructor of interruptibleThread
	//initialize an internal thread and set the flag.
    template<typename FuncType>
    InterruptibleThread(FuncType f) {
        std::promise<FlagType> p;
        internalThread = std::thread([&]{
            p.set_value(&ThreadInterruptFlag);
			
			//try and catch if there is an exception
            try {
                f();
            } catch (ProgramException& e) {
                DEBUG_PRINTF("%s\n", e.what());
            }
        });
        interruptFlag = p.get_future().get();
    }

	//safe deconstructor
    ~InterruptibleThread() {
        if (internalThread.joinable()) {
            internalThread.join();
        }
    }

	//detach
    void detach() {
        internalThread.detach();
    }

	//a open interface to interrupt the thread.
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