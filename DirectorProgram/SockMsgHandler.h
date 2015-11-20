#ifndef SOCK_MSG_HANDLER_H
#define SOCK_MSG_HANDLER_H
#include <mutex>

class SockMsgHandler {
public:
    struct RecvMsg {
        enum MsgType { kStart, kStop, kQuit, kOther };
        MsgType type;
        int val;
        RecvMsg(const MsgType& type_, const int& val_ = 0) : type(type_), val(val_) {}
    };

    static SockMsgHandler* instance();
    ~SockMsgHandler();

    RecvMsg Receive(const std::string& msg) const;

    RecvMsg Receive(char* msg) const;

private:
    bool Validate(const std::string& msg, const RecvMsg::MsgType& msg_type) const;

    static SockMsgHandler* handler_;
    static std::once_flag once_flag_;
};

#endif
