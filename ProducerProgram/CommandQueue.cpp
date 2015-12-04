// CommandQueue.cpp -- Defines a singleton command queue used in this program
// A thread safe queue to store user input commands.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao, all rights reserved.
//
#include "stdafx.h"
#include "CommandQueue.h"

CommandQueue* CommandQueue::queue_ = nullptr;
std::once_flag CommandQueue::once_flag_;

CommandQueue* CommandQueue::instance() {
    std::call_once(once_flag_, []{ queue_ = new CommandQueue; });
    return queue_;
}

CommandQueue::CommandQueue() {}

CommandQueue::CommandQueue(const CommandQueue&) {}

CommandQueue& CommandQueue::operator=(const CommandQueue&) const {
    return *queue_;
}
