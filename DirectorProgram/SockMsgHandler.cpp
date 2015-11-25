#include "stdafx.h"
#include "SockMsgHandler.h"
#include "utils.h"
#include "RdWrSockSvcHandler.h"
#include <vector>
#include <numeric>

SockMsgHandler* SockMsgHandler::handler_ = nullptr;
std::once_flag SockMsgHandler::once_flag_;

SockMsgHandler* SockMsgHandler::instance() {
    std::call_once(once_flag_, []{ handler_ = new SockMsgHandler; });
    return handler_;
}

SockMsgHandler::SockMsgHandler(): 
    feedback_proxy_(nullptr) 
{}

SockMsgHandler::~SockMsgHandler() {
    if (handler_) delete this;
}

SockMsgHandler::RecvMsg SockMsgHandler::Receive(const std::string& msg) const {
    if (Validate(msg, RecvMsg::kStart))
        return RecvMsg(RecvMsg::kStart, to_number(tokenize(msg).back()));
    if (Validate(msg, RecvMsg::kStop))
        return RecvMsg(RecvMsg::kStop, to_number(tokenize(msg).back()));
    if (Validate(msg, RecvMsg::kQuit))
        return RecvMsg(RecvMsg::kQuit);
    return RecvMsg(RecvMsg::kOther);
}

SockMsgHandler::RecvMsg SockMsgHandler::Receive(char* msg) const {
    return Receive(std::string(msg));
}

void SockMsgHandler::set_feedback_proxy(RdWrSockSvcHandler* feedback_proxy) {
    feedback_proxy_ = feedback_proxy;
}

// format: PLAYLIST <size> <script> {<script>}
void SockMsgHandler::FeedbackPlayList(std::vector<std::string> plays) const {
    const std::string sPlayListHeader = "PLAYLIST";
    const std::string sSize = std::to_string(plays.size());
    const std::string sPlayList = std::accumulate(plays.begin(), plays.end(), std::string(),
        [](const std::string& a, std::string b)
    {
        return a.empty() ? GetFileName(b) : a + " " + GetFileName(b);
    });
    const std::string msg = sPlayListHeader + " " + sSize + " " + sPlayList + '\0';
    SendFeedback(msg);
}

void SockMsgHandler::FeedbackStatus(bool ready, unsigned cur_idx) const {
    const std::string sStatusHeader = "STATUS";
    const std::string sReady = ready ? "available" : "unavailable";
    const std::string sIndex = ready ? "" : std::to_string(cur_idx);
    const std::string msg = sStatusHeader + " " + sReady + " " + sIndex + '\0';
    SendFeedback(msg);
}

bool SockMsgHandler::Validate(const std::string& msg, const RecvMsg::MsgType& msg_type) const{
    const std::string sStartHeader = "start";
    const std::string sStopHeader = "stop";
    const std::string sQuitHeader = "quit";
    const unsigned kStartMsgSize = 2;
    const unsigned kStopMsgSize = 2;
    
    std::vector<std::string> msg_tokens = tokenize(msg);
    if (msg_tokens.empty()) { return false; }

    switch (msg_type) {
    case RecvMsg::kStart: 
        return msg_tokens.front() == sStartHeader && 
               msg_tokens.size() == kStartMsgSize && 
               is_number(msg_tokens.back());
    case RecvMsg::kStop:
        return msg_tokens.front() == sStopHeader &&
               msg_tokens.size() == kStopMsgSize &&
               is_number(msg_tokens.back());
    case RecvMsg::kQuit: 
        return msg_tokens.front() == sQuitHeader;
    case RecvMsg::kOther:
    default: 
        return false;
    }
}

void SockMsgHandler::SendFeedback(const std::string& msg) const {
    int ret = feedback_proxy_->peer().send_n(msg.c_str(), msg.size());
    if (ret < 0) {
        // FIXME handle this situation.
    }
}
