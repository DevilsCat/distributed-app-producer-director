//
//Signal Event Handler
//This class is inherited from ACE_EVENT_HANDLER to handle the signal interrupt Ctrl-C
//


#ifndef SIGNAL_EVENT_HANDLER_H
#define SIGNAL_EVENT_HANDLER_H
#include <ace/Event_Handler.h>
#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include "utils.h"

class SignalEventHandler : public ACE_Event_Handler {
public:
    
	//default constructor
	//have some debug info printed
    SignalEventHandler() {
        DEBUG_PRINTF("[%p]SignalEventHandler Allocated\n", this);
    }

	//default deconstructor
	//have some debug info printed
    ~SignalEventHandler() {
        DEBUG_PRINTF("[%p]SignalEventHandler Deallocated\n", this);
    }

	//override handle_signal deal with ctrl c 
	//and when user hit ctrl c, it should end the event loop
    virtual int handle_signal(int signum, siginfo_t*, ucontext_t*) override {
        if (signum != SIGINT) return 0;

        DEBUG_PRINTF("CTRL-C Interrupt\n");
        ACE_Reactor::instance()->end_event_loop();

        return 0;
    }
};

#endif