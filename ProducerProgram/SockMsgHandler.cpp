#include "stdafx.h"
#include "SockMsgHandler.h"
#include "Utils.h"

#define 	AVAILABLESIZE 2;
#define		UNAVAILABLESIZE 3;


SockMsgHandler* SockMsgHandler::handler_ = nullptr;
std::once_flag SockMsgHandler::once_flag_;

SockMsgHandler* SockMsgHandler::instance() {
	std::call_once(once_flag_, []{ handler_ = new SockMsgHandler; });
	return handler_;
}

SockMsgHandler::~SockMsgHandler() {
	if (handler_) delete this;
}

bool SockMsgHandler::Validate(const MsgType& type, std::vector<std::string>& MsgToken) {
	if (MsgToken.empty()) { return false; }

	const std::string sStatus = "STATUS";
	const std::string sAvailable = "available";
	const std::string sUnavailable = "unavailable"; 
	const std::string sPlaylist = "PLAYLIST";

	const size_t kAvailableSize = 2;
	const size_t kUnavailableSize = 3;

	switch (type) {		//Fix hard code
	case MsgType::kStatus: {
		bool available = MsgToken[1] == sAvailable && MsgToken.size() == kAvailableSize;
		bool unavailable = MsgToken.size() == kUnavailableSize && MsgToken[1] == sUnavailable && utils::is_number(MsgToken[2]);
		return MsgToken.front() == sStatus && (available || unavailable);
	}
	case MsgType::kPlaylist: {
		return MsgToken.front() == sPlaylist && utils::is_number(MsgToken[1]) && MsgToken.size() > 2; 
	}
	default:
		return false;
	}
}

std::string SockMsgHandler::MakeStartMsg(const int& play_id) {
    const std::string sStartHeader("start");
    const std::string sStartPlayIdx(std::to_string(play_id));
    return sStartHeader + " " + sStartPlayIdx;
}
