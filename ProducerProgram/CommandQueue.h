#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H
#include "ThreadSafeQueue.h"
#include "Commands.h"

// A singleton command queue used in this program
class CommandQueue : public ThreadSafeQueue<std::shared_ptr<Command>> {
public:
    static CommandQueue* instance();

private:
    CommandQueue();
    CommandQueue(const CommandQueue&);
    CommandQueue& operator=(const CommandQueue&) const;;
    
    static CommandQueue* queue_;
    static std::once_flag once_flag_;
};

#endif