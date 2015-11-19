#ifndef RD_WR_SVC_HANDLER_H
#define RD_WR_SVC_HANDLER_H
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include "Director.h"
#include "SignalEventHandler.h"

class RdWrSockSvcHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>   {
public:
    RdWrSockSvcHandler() : director_(nullptr) {}

    RdWrSockSvcHandler(Director* director) : director_(director) {
        ACE_DEBUG((LM_INFO, "[%x]RdWrSockSvcHandler Allocated\n", this));
    }

    ~RdWrSockSvcHandler() {
        ACE_DEBUG((LM_INFO, "[%x]RdWrSockSvcHandler Deallocated\n", this));
    }

    virtual int open(void* acceptor_or_connector) override {
        ACE_DEBUG((LM_INFO, "[%x]RdWrSockSvcHandler Connection Established\n", this));
        ACE_Reactor::instance()->register_handler(this, READ_MASK);
        ACE_Reactor::instance()->register_handler(SIGINT, new SignalEventHandler);
        return 0;
    }

    virtual int handle_input(ACE_HANDLE fd) override {
        char buf[20];
        int ret = peer().recv(buf, 20);
        if (ret > 0) {
            buf[ret-1] = '\0';
            std::string str(buf);
            ACE_DEBUG((LM_INFO, "%s\n", str.c_str()));
            if (str == "start")
                director_->Start(0);
            else if (str == "stop")
                director_->Stop();
            else if (str == "quit") {
                ACE_Reactor::instance()->end_event_loop();
            }
        }
        return 0;
    }

private:
    Director* director_;
};

#endif

