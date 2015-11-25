#include "stdafx.h"
#include "SockMsgHandler.h"
#include "Utils.h"

#define 	AVAILABLESIZE 2
#define		UNAVAILABLESIZE 3
#define		PLAYLISTSIZE	2
#define		STATUS_POS	1
#define		PLAY_POS	2
#define		PLAY_NUM_POS	1	

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

	switch (type) {
	case MsgType::kStatus: {
		bool available = MsgToken[STATUS_POS] == sAvailable && MsgToken.size() == AVAILABLESIZE;
		bool unavailable = MsgToken.size() == UNAVAILABLESIZE && MsgToken[STATUS_POS] == sUnavailable && utils::is_number(MsgToken[PLAY_POS]);
		return MsgToken.front() == sStatus && (available || unavailable);
	}
	case MsgType::kPlaylist: {
		return MsgToken.front() == sPlaylist && utils::is_number(MsgToken[PLAY_NUM_POS]) && MsgToken.size() > PLAYLISTSIZE;
	}
	default:
		return false;
	}
}