#ifndef SIGNAL_EVENT_HANDLER_H
#define SIGNAL_EVENT_HANDLER_H
#include <ace/Event_Handler.h>
#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include "utils.h"

class SignalEventHandler : public ACE_Event_Handler {
public:
    

    SignalEventHandler() {
        DEBUG_PRINTF("[%p]SignalEventHandler Allocated\n", this);
    }

    ~SignalEventHandler() {
        DEBUG_PRINTF("[%p]SignalEventHandler Deallocated\n", this);
    }

    virtual int handle_signal(int signum, siginfo_t*, ucontext_t*) override {
        if (signum != SIGINT) return 0;

        DEBUG_PRINTF("CTRL-C Interrupt\n");
        ACE_Reactor::instance()->end_event_loop();

        return 0;
    }
};

#endif