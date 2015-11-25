// Message.h -- Declares a Message Class which is used as a protocal to establish communication between
// invocator and executor.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#ifndef MESSAGE_H
#define MESSAGE_H

#include <future>
#include <atomic>
#include "ProgramException.h"

// This class is Message, the thing that director send to the queue in the active object. 
// so that the player can know which part he is in charge of next. 
template <typename FutureType>
class Message : public std::promise<FutureType> {
    typedef std::string KeyType;
public:
    Message(const unsigned msg_code) : 
        msg_code_(msg_code), done_flag_(new std::atomic<bool>(false))
    {}

    Message(Message &&other) :
        std::promise<FutureType>(std::move(other)),
        msg_code_(std::move(other.msg_code_)), 
        str_map_(std::move(other.str_map_)),
        unsigend_map_(std::move(other.unsigend_map_)),
        done_flag_(std::move(other.done_flag_))
    {}

    Message &operator= (Message &&other) {
        msg_code_ = std::move(other.msg_code_);
        str_map_ = std::move(other.str_map_);
        unsigend_map_ = std::move(other.unsigend_map_);
        done_flag_ = std::move(other.done_flag_);
        return *this;
    }

    void SetStrMessage(const KeyType &key, const std::string &str) {
        str_map_[key] = str;
    }

    void SetUnsignedMessage(const KeyType &key, const unsigned &i) {
        unsigend_map_[key] = i;
    }

    const std::string &GetStrMessage(const KeyType& key) {
        return str_map_[key];
    }

    const unsigned &GetUnsignedMessage(const KeyType& key) {
        return unsigend_map_[key];
    }

    const unsigned &What() const { 
        return msg_code_;
    }

    std::future<FutureType> GetFuture() {
        return std::promise<FutureType>::get_future();
    }

    void SetPromiseValue(const FutureType& val) {
        std::promise<FutureType>::set_value(val);
    }

    void SetPromiseValueAtThreadExit(const FutureType &val) {
        std::promise<FutureType>::set_value_at_thread_exit(val);
    }

    void SetPromiseException(ProgramException& e) {
        std::promise<FutureType>::set_exception(std::make_exception_ptr(e));
    }

    void SetPromiseExceptionAtThreadExit(ProgramException& e) {
        std::promise<FutureType>::set_exception_at_thread_exit(std::make_exception_ptr(e));
    }

    std::shared_ptr<std::atomic<bool>> get_done_flag() const { return done_flag_; }

    void set_done_flag() { done_flag_->store(true); }

    static std::shared_ptr<Message> MakeMessage(const unsigned msg_code) {
        return std::make_shared<Message>(msg_code);
	}

private:
    Message() : msg_code_() {};
    Message(const Message &other) : msg_code_(other.msg_code_) {};
    Message &operator= (const Message &) { return *this; };

    const unsigned msg_code_;
    std::map<KeyType, std::string> str_map_;
    std::map<KeyType, unsigned> unsigend_map_;

    std::shared_ptr<std::atomic<bool>> done_flag_;

};

#endif 