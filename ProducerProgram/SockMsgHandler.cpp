#include "stdafx.h"
#include "SockMsgHandler.h"
#include "Utils.h"

SockMsgHandler* SockMsgHandler::handler_ = nullptr;
std::once_flag SockMsgHandler::once_flag_;

SockMsgHandler::RecvMsg::RecvMsg(const MsgType& type): type (type) {}

SockMsgHandler::RecvMsg::RecvMsg(const MsgType& type, std::vector<std::string>& MsgToken): 
	type(type), plays(MsgToken) {}

SockMsgHandler* SockMsgHandler::instance() {
	std::call_once(once_flag_, []{ handler_ = new SockMsgHandler; });
	return handler_;
}

SockMsgHandler::~SockMsgHandler() {
	if (handler_) delete this;
}

SockMsgHandler::RecvMsg SockMsgHandler::Recive(const std::string& msg) {
	std::vector<std::string> MsgToken = utils::tokenize(msg);

	if (Validate(msg, MsgType::kPlaylist, MsgToken))
		return RecvMsg(MsgType::kPlaylist, MsgToken);
	if (Validate(msg, MsgType::kAvailable, MsgToken))
		return RecvMsg(MsgType::kAvailable, MsgToken);
	if (Validate(msg, MsgType::kUnavailable, MsgToken))
		return RecvMsg(MsgType::kUnavailable, MsgToken);
	return RecvMsg(MsgType::kOther);
}

bool SockMsgHandler::Validate(const std::string& msg, const MsgType& type, std::vector<std::string>& MsgToken) {
	if (MsgToken.empty()) { return false; }

	const std::string sAvailable = "AVAILABLE";
	const std::string sUnavailable = "UNAVAILABLE"; 
	const std::string sPlaylist = "PLAYLIST";
	const unsigned kAvailableMsgSize = 2;

	switch (type) {
	case MsgType::kAvailable:
		return MsgToken.front() == sAvailable && MsgToken.size() == kAvailableMsgSize;
	case MsgType::kUnavailable:
		return MsgToken.front() == sUnavailable && utils::is_number(MsgToken[1]) && MsgToken.size() == 3;
	case MsgType::kPlaylist:
		return MsgToken.front() == sPlaylist;		//FIXME filename validate
	default:
		return false;
	}
}