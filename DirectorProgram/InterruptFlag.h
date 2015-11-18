// InterruptFlag.h -- Declares a thread local interruption flag used for interrupt thread.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#ifndef INTERRUPT_FLAG_H
#define INTERRUPT_FLAG_H
#include <condition_variable>

// Thread local flag.
extern __declspec(thread) bool ThreadInterruptFlag;

//
// InterruptionPoint()
// Checks if the flag has been set, throw an error for exiting, if true.
//
void InterruptionPoint();

//
// InterruptibleWait()
// Subsititudes the original uninterruptible wait, which allows thread to detect an interruption.
//
template<typename Predicate>
void InterruptibleWait(std::condition_variable& cond, std::unique_lock<std::mutex>& lk, Predicate pred) {
    while (!cond.wait_for(lk, std::chrono::milliseconds(10), [&]{ return ThreadInterruptFlag || pred(); })) {}
    InterruptionPoint();
}

#endif