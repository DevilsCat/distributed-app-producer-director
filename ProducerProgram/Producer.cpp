#include "stdafx.h"
#include "Producer.h"
#include <ace/Log_Msg.h>
#include "RdWrServiceHandler.h"
#include <ace/Reactor.h>
#include <ace/Proactor.h>

Producer* Producer::producer_ = nullptr;
std::once_flag Producer::once_flag_;

Producer* Producer::instance() {
    std::call_once(once_flag_, []{ producer_ = new Producer; });
    return producer_;
}

Producer::~Producer() {
    if (producer_)
        delete producer_;
}

void Producer::AddHandler(RdWrServiceHandler* handler) {
    handlers_.push_back(handler);
}

void Producer::RemoveHandler(RdWrServiceHandler* handler) {
    std::vector<RdWrServiceHandler*>::iterator it = find(handlers_.begin(), handlers_.end(), handler);
    if (it == handlers_.end()) { return; }  // FIXME this is a very big logic error.
    handlers_.erase(it);
}

void Producer::Start(const unsigned& num) {
    ACE_DEBUG((LM_INFO, "Producer: Executing start <%d> command.\n", num));
    // Check handlers index is in bound, otherwise simply returns.
    // Select handler by given index.
    // Call send method in the handler to send message to that Director.
    if (handlers_.size() <= num){ return; }  //FIXME do we want to handle this using exception
    std::string msg("test");
    handlers_[num]->InvokeSend(msg);
}

void Producer::Stop(const unsigned& num) {
    ACE_DEBUG((LM_INFO, "Producer: Executing stop <%d> command.\n", num));
    if (handlers_.size() <= num){ return; }  //FIXME do we want to handle this using exception
    std::string msg("test");
    handlers_[num]->InvokeSend(msg);
}

void Producer::Quit() {
    ACE_DEBUG((LM_INFO, "Producer: Executing quit command.\n"));

    // Wait on all Director quit.

    // Quit itself
    ACE_Reactor::instance()->end_event_loop();
    ACE_Proactor::instance()->end_event_loop();
}
