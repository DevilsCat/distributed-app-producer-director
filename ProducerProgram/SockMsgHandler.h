#ifndef  MSGHANDLER_H
#define  MSGHANDLER_H

#include <string>
#include <vector>
#include <mutex>
#include <iostream>

class SockMsgHandler {
public:
	enum MsgType{ kAvailable, kUnavailable, kPlaylist, kOther };

	struct RecvMsg {
		MsgType type;
		std::vector<std::string> plays;

		RecvMsg(const MsgType& type);
		RecvMsg(const MsgType& type, std::vector<std::string>& MsgToken);

		void print() {
			std::cout << "Type: " << type << "Plays: ";
			for (auto p : plays) {
				std::cout << p << std::ends;
			}
			std::cout << std::endl;
		}
	};

	static SockMsgHandler* instance();
	~SockMsgHandler();

	RecvMsg Recive(const std::string& msg);


private:
	bool Validate(const std::string& msg, const MsgType& type, std::vector<std::string>& msgToken);
	static SockMsgHandler* handler_;
	static std::once_flag once_flag_;
};

#endif
