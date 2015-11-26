//
//Define two derived class of ACE_Event_Handler. 
//CommandEventHandler:  event handler for user's input commands.
//SignalEventHandler: event handler for Ctrl-C signal.
//
#ifndef REACTOR_EVENT_HANDLERS_H
#define REACTOR_EVENT_HANDLERS_H
#include <ace/Event_Handler.h>
#include "CommandQueue.h"
#include "Producer.h"

class CommandEventHandler : public ACE_Event_Handler {
public:
    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) override {
        delete this;
        return 0;
    }
	//
	//handle_timeout()
	//pop a command from command queue and execute.
	//
    virtual int handle_timeout(const ACE_Time_Value& current_time, const void* act) override {
        std::shared_ptr<Command> command;
        if (CommandQueue::instance()->try_pop(command)) {
            command->ExecuteOn(*Producer::instance());
        }
        return 0;
    }
};

class SignalEventHandler : public ACE_Event_Handler {
public:
    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) override {
        delete this;
        return 0;
    }
	//
	//handle_signal()
	//push SIGINT into command queue.
	//
    virtual int handle_signal(int signum, siginfo_t*, ucontext_t*) override {
        if (signum != SIGINT) { return 0; }  // ignore signal int
        CommandQueue::instance()->push(std::make_shared<QuitCommand>());
        return 0;
    }
};
#endif