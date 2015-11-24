#ifndef RD_WR_SVC_HANDLER_H
#define RD_WR_SVC_HANDLER_H
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include "Director.h"
#include "SignalEventHandler.h"
#include "SockMsgHandler.h"
#include "utils.h"

class RdWrSockSvcHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>   {
public:
    RdWrSockSvcHandler() : director_(nullptr) {}

    RdWrSockSvcHandler(Director* director) : director_(director) {
        DEBUG_PRINTF("[%p]RdWrSockSvcHandler Allocated\n", this);
    }

    ~RdWrSockSvcHandler() {
        DEBUG_PRINTF("[%p]RdWrSockSvcHandler Deallocated\n", this);
    }

    virtual int open(void* acceptor_or_connector) override {
        DEBUG_PRINTF("[%p]RdWrSockSvcHandler Connection Established\n", this);
        ACE_Reactor::instance()->register_handler(this, READ_MASK);
        ACE_Reactor::instance()->register_handler(SIGINT, new SignalEventHandler);
        ACE_Reactor::instance()->schedule_timer(  // director can now run the state machine
            director_,
            nullptr,
            ACE_Time_Value(),
            ACE_Time_Value(0, 10000)  // FIXME should not hard code.
        );
        SockMsgHandler::instance()->set_feedback_proxy(this);
        return 0;
    }

    virtual int handle_input(ACE_HANDLE fd) override {
        char buf[20];  // FIXME buf 20 is not enough.
        int ret = peer().recv(buf, 20);
        if (ret > 0) {
            std::string str(buf);
            DEBUG_PRINTF("%s\n", str.c_str());
            
            // Execute the received message on Director object.
            SockMsgHandler::RecvMsg msg = SockMsgHandler::instance()->Receive(str);
            switch(msg.type) {
            case SockMsgHandler::RecvMsg::kStart: 
                director_->set_request_play_idx(msg.val);
                director_->run(inStart);
                break;
            case SockMsgHandler::RecvMsg::kStop: 
                director_->set_request_play_idx(msg.val);
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

