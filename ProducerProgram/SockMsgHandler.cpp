//
// Defines a singleton class to analyse message from directors and construct instruction
// to be sent to directors.
//
#include "stdafx.h"
#include "SockMsgHandler.h"
#include "Utils.h"

SockMsgHandler* SockMsgHandler::handler_ = nullptr;
std::once_flag SockMsgHandler::once_flag_;

SockMsgHandler* SockMsgHandler::instance() {
    std::call_once(once_flag_, []{ handler_ = new SockMsgHandler; });
    return handler_;
}

SockMsgHandler::~SockMsgHandler() {
    if (handler_) delete this;
}

bool SockMsgHandler::Validate(const FeedBackMsgType& type, std::vector<std::string>& MsgToken) {
    if (MsgToken.empty()) { return false; }

    const std::string sStatus = "STATUS";
    const std::string sAvailable = "available";
    const std::string sUnavailable = "unavailable"; 
    const std::string sPlaylist = "PLAYLIST";

    const size_t kAvailableSize = 2;
    const size_t kUnavailableSize = 3;

    switch (type) { //Fixed hard code
    case FeedBackMsgType::kStatus: {
        bool available = MsgToken[1] == sAvailable && MsgToken.size() == kAvailableSize;
        bool unavailable = MsgToken.size() == kUnavailableSize && MsgToken[1] == sUnavailable && utils::is_number(MsgToken[2]);
        return MsgToken.front() == sStatus && (available || unavailable);
    }
    case FeedBackMsgType::kPlaylist: {
        return MsgToken.front() == sPlaylist && utils::is_number(MsgToken[1]) && MsgToken.size() > 2; 
    }
    default:
        return false;
    }
}

std::string SockMsgHandler::MakeSendMsg(SendMsgType type, const int& play_id) {
    switch (type) {
    case kStart: { return MakeStartMsg_(play_id); }
    case kStop:  { return MakeStopMsg_(play_id); }
    case kQuit:  { return MakeQuitMsg_(); }
    default:     { return std::string(); }
    }
}

std::string SockMsgHandler::MakeStartMsg_(const int& play_id) {
    const std::string sStartHeader("start");
    const std::string sStartPlayIdx(std::to_string(play_id));
    return sStartHeader + " " + sStartPlayIdx;
}

std::string SockMsgHandler::MakeStopMsg_(const int& play_id) {
    const std::string sStopHeader("stop");
    const std::string sStopPlayIdx(std::to_string(play_id));
    return sStopHeader + " " + sStopPlayIdx;
}

std::string SockMsgHandler::MakeQuitMsg_() {
    const std::string sQuitHeader("quit");
    return sQuitHeader;
}
