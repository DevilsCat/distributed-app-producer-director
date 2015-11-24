#ifndef  MSGHANDLER_H
#define  MSGHANDLER_H

#include <vector>
#include <mutex>

class SockMsgHandler {
public:
	enum FeedBackMsgType{ kStatus, kPlaylist, kOther };
    enum SendMsgType { kStart, kStop, kQuit };

	static SockMsgHandler* instance();
	~SockMsgHandler();

	bool Validate(const FeedBackMsgType& type, std::vector<std::string>& msgToken);

    std::string MakeSendMsg(SendMsgType type, const int& play_id = -1);
private:
    static std::string MakeStartMsg_(const int& play_id);
    static std::string MakeStopMsg_(const int& play_id);
    static std::string MakeQuitMsg_();

	static SockMsgHandler* handler_;
	static std::once_flag once_flag_;
};
#endif
