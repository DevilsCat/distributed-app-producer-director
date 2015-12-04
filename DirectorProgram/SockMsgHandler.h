//Socket Message Handler
//this class is designed to handle the transition protocal(message)
//


#ifndef SOCK_MSG_HANDLER_H
#define SOCK_MSG_HANDLER_H
#include <mutex>
#include <vector>

class RdWrSockSvcHandler;

class SockMsgHandler {
public:
    struct RecvMsg {
        enum MsgType { kStart, kStop, kQuit, kOther };
        MsgType type;
        int val;
        RecvMsg(const MsgType& type_, const int& val_ = 0) : type(type_), val(val_) {}
    };

    static SockMsgHandler* instance();
    SockMsgHandler();
    ~SockMsgHandler();

    // Receive and interpret message sent from Producer. 
    RecvMsg Receive(const std::string& msg) const;
    RecvMsg Receive(char* msg) const;

    // Send message to client.
    void set_feedback_proxy(RdWrSockSvcHandler *);
    void FeedbackPlayList(std::vector<std::string> plays) const;
    void FeedbackStatus(bool ready, unsigned cur_idx = 0) const;

private:
    bool Validate(const std::string& msg, const RecvMsg::MsgType& msg_type) const;
    void SendFeedback(const std::string& msg) const;

    static SockMsgHandler* handler_;
    static std::once_flag once_flag_;

    RdWrSockSvcHandler* feedback_proxy_;
};

#endif
