#ifndef RD_WR_SVC_HANDLER_H
#define RD_WR_SVC_HANDLER_H
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include "Director.h"
#include "SignalEventHandler.h"
#include "SockMsgHandler.h"

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
        char buf[20];  // FIXME buf 20 is not enough.
        int ret = peer().recv(buf, 20);
        if (ret > 0) {
            std::string str(buf);
            ACE_DEBUG((LM_INFO, "%s\n", str.c_str()));
            
            // Execute the received message on Director object.
            SockMsgHandler::RecvMsg msg = SockMsgHandler::instance()->Receive(str);
            switch(msg.type) {
            case SockMsgHandler::RecvMsg::kStart: 
                director_->set_play_idx(msg.val);
                director_->run(inStart);
                break;
            case SockMsgHandler::RecvMsg::kStop: 
                director_->run(inStop);
                break;
            case SockMsgHandler::RecvMsg::kQuit: 
                director_->run(inQuit);
                break;
            case SockMsgHandler::RecvMsg::kOther:
            default: 
                break;
            }
        }
        return 0;
    }

private:
    Director* director_;
};

#endif

