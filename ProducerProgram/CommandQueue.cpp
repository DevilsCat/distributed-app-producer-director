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
