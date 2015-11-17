#ifndef COMMAND_EVENT_HANDLER_H
#define COMMAND_EVENT_HANDLER_H
#include <ace/Event_Handler.h>
#include "CommandQueue.h"
#include "Producer.h"

class CommandEventHandler : public ACE_Event_Handler {

public:
    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) override {
        delete this;
        return 0;
    }

    virtual int handle_timeout(const ACE_Time_Value& current_time, const void* act) override {
        std::shared_ptr<Command> command;
        if (CommandQueue::instance()->try_pop(command)) {
            command->ExecuteOn(*Producer::instance());
        }
        return 0;
    }
};

#endif