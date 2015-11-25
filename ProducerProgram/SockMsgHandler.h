#ifndef  MSGHANDLER_H
#define  MSGHANDLER_H

#include <string>
#include <vector>
#include <mutex>

class SockMsgHandler {
public:
	enum MsgType {
		kStatus, 
		kPlaylist, 
		kOther
	};
	//
	//instance()
	//unique instance of the class
	//
	static SockMsgHandler* instance();
	//
	//Public deconstructor
	//delete the object when project is terminated.
	//
	~SockMsgHandler();
	//
	//Validate()
	//check if the message recived from director is in valid form.
	//
	bool Validate(const MsgType& type, std::vector<std::string>& msgToken);
private:
	//
	static SockMsgHandler* handler_;
	//once flag to make sure unique instance.
	static std::once_flag once_flag_;
};

#endif
