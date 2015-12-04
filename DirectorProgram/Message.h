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
	
	//the message type is string
    typedef std::string KeyType;
public:

	//constructor
	//by default all is not done yet.
    Message(const unsigned msg_code) : 
        msg_code_(msg_code), done_flag_(new std::atomic<bool>(false))
    {}

	//move to copy
    Message(Message &&other) :
        std::promise<FutureType>(std::move(other)),
        msg_code_(std::move(other.msg_code_)), 
        str_map_(std::move(other.str_map_)),
        unsigend_map_(std::move(other.unsigend_map_)),
        done_flag_(std::move(other.done_flag_))
    {}

	//override operator = 
    Message &operator= (Message &&other) {
        msg_code_ = std::move(other.msg_code_);
        str_map_ = std::move(other.str_map_);
        unsigend_map_ = std::move(other.unsigend_map_);
        done_flag_ = std::move(other.done_flag_);
        return *this;
    }

	//set string type message
    void SetStrMessage(const KeyType &key, const std::string &str) {
        str_map_[key] = str;
    }

	//set unsigned type message
    void SetUnsignedMessage(const KeyType &key, const unsigned &i) {
        unsigend_map_[key] = i;
    }

	//get the string message given the key
    const std::string &GetStrMessage(const KeyType& key) {
        return str_map_[key];
    }

	//get the unsigned type message given the key
    const unsigned &GetUnsignedMessage(const KeyType& key) {
        return unsigend_map_[key];
    }

	//return the message code 
    const unsigned &What() const { 
        return msg_code_;
    }

	//get the future vector
    std::future<FutureType> GetFuture() {
        return std::promise<FutureType>::get_future();
    }

	//set promise value
    void SetPromiseValue(const FutureType& val) {
        std::promise<FutureType>::set_value(val);
    }

	//set promise value when the thread exit
    void SetPromiseValueAtThreadExit(const FutureType &val) {
        std::promise<FutureType>::set_value_at_thread_exit(val);
    }

	//set promise exception
    void SetPromiseException(ProgramException& e) {
        std::promise<FutureType>::set_exception(std::make_exception_ptr(e));
    }

	//set promise exception when thread exits.
    void SetPromiseExceptionAtThreadExit(ProgramException& e) {
        std::promise<FutureType>::set_exception_at_thread_exit(std::make_exception_ptr(e));
    }

	//get the done_flag vector
    std::shared_ptr<std::atomic<bool>> get_done_flag() const { return done_flag_; }

	//set the done_flag
    void set_done_flag() { done_flag_->store(true); }

	//make a message(shared pointer)
    static std::shared_ptr<Message> MakeMessage(const unsigned msg_code) {
        return std::make_shared<Message>(msg_code);
	}

private:

	//private constructor 
    Message() : msg_code_() {};

	//private constructor
    Message(const Message &other) : msg_code_(other.msg_code_) {};

	//private operate so it cannot be called outside 
    Message &operator= (const Message &) { return *this; };

    const unsigned msg_code_;
	
	//a map that stores string type message
    std::map<KeyType, std::string> str_map_;
	
	//a map that stores unsigned type message
    std::map<KeyType, unsigned> unsigend_map_;

	//done flags that represent if all the messages are done
    std::shared_ptr<std::atomic<bool>> done_flag_;

};

#endif 