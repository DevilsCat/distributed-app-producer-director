//
// Defined a singleton command queue used in this program
// A thread safe queue to store user input commands. 
//
#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H
#include "ThreadSafeQueue.h"
#include "Commands.h"

class CommandQueue : public ThreadSafeQueue<std::shared_ptr<Command>> {
public:
    static CommandQueue* instance();

private:
	//
	//Define copy constructor and copy-assignment operator as private to make sure
	//unique copy of producer class.
	//
    CommandQueue();
    CommandQueue(const CommandQueue&);
    CommandQueue& operator=(const CommandQueue&) const;;
    //Used for singleton pattern
    static CommandQueue* queue_;
    static std::once_flag once_flag_;
};

#endif