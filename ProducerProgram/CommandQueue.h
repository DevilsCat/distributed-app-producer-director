#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H
#include "ThreadSafeQueue.h"
#include "Commands.h"

// A singleton command queue used in this program
class CommandQueue : public ThreadSafeQueue<std::shared_ptr<Command>> {
public:
    static CommandQueue* instance() {
        std::call_once(once_flag_, []{ queue_ = new CommandQueue; });
        return queue_;
    }
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;

private:
    CommandQueue() = default;

    static CommandQueue* queue_;
    static std::once_flag once_flag_;
};

#endif