#include "stdafx.h"
#include "SockMsgHandler.h"
#include "utils.h"
#include <vector>

SockMsgHandler* SockMsgHandler::handler_ = nullptr;
std::once_flag SockMsgHandler::once_flag_;

SockMsgHandler* SockMsgHandler::instance() {
    std::call_once(once_flag_, []{ handler_ = new SockMsgHandler; });
    return handler_;
}

SockMsgHandler::~SockMsgHandler() {
    if (handler_) delete this;
}

SockMsgHandler::RecvMsg SockMsgHandler::Receive(const std::string& msg) const {
    if (Validate(msg, RecvMsg::kStart))
        return RecvMsg(RecvMsg::kStart, to_number(tokenize(msg).back()));
    if (Validate(msg, RecvMsg::kStop))
        return RecvMsg(RecvMsg::kStop);
    if (Validate(msg, RecvMsg::kQuit))
        return RecvMsg(RecvMsg::kQuit);
    return RecvMsg(RecvMsg::kOther);
}

SockMsgHandler::RecvMsg SockMsgHandler::Receive(char* msg) const {
    return Receive(std::string(msg));
}

bool SockMsgHandler::Validate(const std::string& msg, const RecvMsg::MsgType& msg_type) const{
    const std::string sStartHeader = "start";
    const std::string sStopHeader = "stop";
    const std::string sQuitHeader = "quit";
    const unsigned kStartMsgSize = 2;
    
    std::vector<std::string> msg_tokens = tokenize(msg);
    if (msg_tokens.empty()) { return false; }

    switch (msg_type) {
    case RecvMsg::kStart: 
        return msg_tokens.front() == sStartHeader && 
               msg_tokens.size() == kStartMsgSize && 
               is_number(msg_tokens.back());
    case RecvMsg::kStop:
        return msg_tokens.front() == sStopHeader;
    case RecvMsg::kQuit: 
        return msg_tokens.front() == sQuitHeader;
    case RecvMsg::kOther:
    default: 
        return false;
    }
}
