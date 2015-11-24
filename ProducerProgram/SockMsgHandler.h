#ifndef  MSGHANDLER_H
#define  MSGHANDLER_H

#include <vector>
#include <mutex>

class SockMsgHandler {
public:
	enum MsgType{ kStatus, kPlaylist, kOther };

	static SockMsgHandler* instance();
	~SockMsgHandler();

	bool Validate(const MsgType& type, std::vector<std::string>& msgToken);

    std::string MakeStartMsg(const int& play_id);
private:
	static SockMsgHandler* handler_;
	static std::once_flag once_flag_;
};
#endif
