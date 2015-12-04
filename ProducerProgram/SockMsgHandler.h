//
// Declares a singleton class to analyse message from directors and construct instruction
// to be sent to directors.
//
#ifndef  MSGHANDLER_H
#define  MSGHANDLER_H

#include <vector>
#include <mutex>

class SockMsgHandler {
public:
	//enum for type of message sent by directors.
	enum FeedBackMsgType{ kStatus, kPlaylist, kOther };
    //enum for type of message sent by producer.
	enum SendMsgType { kStart, kStop, kQuit };
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
	//Take a message from director and a message type, chech if the message is that type.
	//
	bool Validate(const FeedBackMsgType& type, std::vector<std::string>& msgToken);
	//
	//MakeSendMsg()
	//Given a message type and id in play menu, call corresponding function to make 
	// a instruction.
	//
    std::string MakeSendMsg(SendMsgType type, const int& play_id = -1);
private:
	//
	//MakeStartMsg_()
	//make a start instruction to be sent to director.
	//
    static std::string MakeStartMsg_(const int& play_id);
    //
	//MakeStopMsg_()
	//make a stop instruction to be sent to director.
	//
	static std::string MakeStopMsg_(const int& play_id);
    //
	//MakeQuitMsg_()
	//make a quit instruction to be sent to director.
	//
	static std::string MakeQuitMsg_();

	//used for singleton pattern.
	static SockMsgHandler* handler_;
	//once flag to make sure unique instance.
	static std::once_flag once_flag_;
};
#endif
