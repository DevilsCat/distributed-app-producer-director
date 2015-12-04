//
//ReadWrite Socket Service Handler 
//declares a service handler that can do both read/write 
//

#ifndef RD_WR_SVC_HANDLER_H
#define RD_WR_SVC_HANDLER_H
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include "Director.h"
#include "SignalEventHandler.h"
#include "SockMsgHandler.h"
#include "utils.h"

#define timeInterval	10000
#define bufferSize		20

class RdWrSockSvcHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>   {
public:
	//default constructor
    RdWrSockSvcHandler() : director_(nullptr) {}

	//constructor given director
    RdWrSockSvcHandler(Director* director) : director_(director) {
        DEBUG_PRINTF("[%p]RdWrSockSvcHandler Allocated\n", this);
    }

	//deconstructor
    ~RdWrSockSvcHandler() {
        DEBUG_PRINTF("[%p]RdWrSockSvcHandler Deallocated\n", this);
    }

	//override open method to register this handler and schedule timer 
    virtual int open(void* acceptor_or_connector) override {
        DEBUG_PRINTF("[%p]RdWrSockSvcHandler Connection Established\n", this);
        ACE_Reactor::instance()->register_handler(this, READ_MASK);
        ACE_Reactor::instance()->register_handler(SIGINT, new SignalEventHandler);
        ACE_Reactor::instance()->schedule_timer(  // director can now run the state machine
            director_,
            nullptr,
            ACE_Time_Value(),
            ACE_Time_Value(0, timeInterval) 
        );
        SockMsgHandler::instance()->set_feedback_proxy(this);
        return 0;
    }

	//override handle_input method to receive message
    virtual int handle_input(ACE_HANDLE fd) override {
        char buf[bufferSize];
        int ret = peer().recv(buf, bufferSize);
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

