#include "stdafx.h"
#include "Producer.h"
#include <ace/Log_Msg.h>

Producer* Producer::producer_ = nullptr;
std::once_flag Producer::once_flag_;

Producer* Producer::instance() {
    std::call_once(once_flag_, []{
        if (producer_ == nullptr)
            producer_ = new Producer;
    });
    return producer_;
}

Producer::~Producer() {
    if (producer_)
        delete producer_;
}

void Producer::Start(const unsigned& num) {
    ACE_DEBUG((LM_INFO, "Producer: Executing start <%d> command.\n", num));
}

void Producer::Stop(const unsigned& num) {
    ACE_DEBUG((LM_INFO, "Producer: Executing stop <%d> command.\n", num));
}

void Producer::Quit() {
    ACE_DEBUG((LM_INFO, "Producer: Executing quit command.\n"));
}
