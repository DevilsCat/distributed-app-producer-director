// ActiveObject.h -- Declares and implements an Active Object which support general purpose of running task in background.
// Also this class are also allows task interruption.
// Created by Yuhan Hao, Anqi Zhang, YuXiao
//
#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H

#include "stdafx.h"
#include "synchronized_list.h"
#include "Message.h"
#include "InterruptibleThread.h"
#include "ProgramException.h"
/* class ActiveObject
* an ActiveObject class template parameterized with a data type parameter T, which has a synchronized_list of T as a
* private member variable, a public default constructor (that default constructs the member variable), a public enqueue
* method that takes a T and calls its synchronized_list member variable's push_front method, and a public RunService
* method that calls the synchronized_list member variable's pop_back method, prints out the value it obtained from that
* call to the standard output stream, and then returns.*/

template<typename T>
class ActiveObject {
public:
    /* akes a pointer to another object of the same type (defaulted to 0 if no pointer is passed) and stores it in a
    * private member variable*/
    ActiveObject() : sync_list_(0, 0), active_f_(false) {}

    virtual ~ActiveObject() {}

    void enqueue(Message<T>& msg) {
        sync_list_.push_front(std::move(msg));
    }

    //
    // Activate()
    // a public Activate method that checks whether or not the object is already active and if it is not constructs a
    // (local) std::thread object with a pointer to the RunService method and a pointer to the current object, and then
    // detaches the thread.
    //
    void Activate() {
        std::lock_guard<std::mutex> lk(mut);
        if (!active_f_) {
            active_f_ = true;
            sync_list_.start_up();
            t_ = std::make_shared<InterruptibleThread>(
                std::bind(&ActiveObject::RunService, this)
            );
        }
    }

    //
    // Deactivate()
    // checks whether or not the object is already active and if it is sets the flag member variable to false and then
    // calls the shut_down method of the synchronized_list, before returning.
    //
    void Deactivate() {
        std::lock_guard<std::mutex> lk(mut);
        if (active_f_) {
            active_f_ = false;
            sync_list_.shut_down();
        }
    }

    //
    // InterruptService()
    // Allows invocator to interrupt the currently Service.
    //
    void InterruptService() {
        t_->interrupt();
    }

    //
    // size()
    // Get the size of the queue
    //
    virtual size_t size() const {
        return sync_list_.size();
    }

protected:
    //
    // Service()
    // User-defined service that want to run in background.
    //
    virtual void Service(std::shared_ptr<Message<T>>) = 0;

private:
    /* RunService()
    * while the flag member variable is true (if not the method should simply return) it repeatedly
    *    (1) calls the synchronized_list member variable's pop_back method, and
    *    (2) prints out the value it obtained from that call, to the standard output stream.
    *
    * if the pointer is zero it prints out each value as before, but if the pointer is non-zero it instead enqueues the
    * value in the active object to which it is pointing (unless there is a cycle of pointers among objects, one of them
    * is at the end of the chain and eventually prints out the values).
    */
    // 10/27/2015: Updated by YuXiao
    //  Added a std::exception_ptr to store the first
    //  exception, and propogate to Main Thread to handle.
    void RunService() {
        std::exception_ptr first_chance_exception_ptr;
        std::once_flag once_exception_flag;
        while (active_f_) {
            msg_ptr_ = sync_list_.pop_back();
            if (msg_ptr_) {
                try {
                    Service(msg_ptr_);
                } catch(ProgramException& e) {
                    // Record the first time exception occurance.
                    std::call_once(once_exception_flag, [&] {
                        first_chance_exception_ptr = make_exception_ptr(e); 
                    });
                    msg_ptr_->SetPromiseException(e);
                }
            }
        }
        if (first_chance_exception_ptr) {
            rethrow_exception(first_chance_exception_ptr);
        }
    }

    // A concurrency list to store and pop Message between invocator and worker.
    synchronized_list<Message<T>> sync_list_;

    /* a private boolean flag member variable (to indicate whether or not the object is active, which the default
    * constructor initializes to false).*/
    bool active_f_;

    /* extra part: a mutex for actvie_f_ */
    std::mutex mut;

    // Stores a pointer for a on-handling message.
    std::shared_ptr<Message<T>> msg_ptr_;

    // Keep track of an interruptible thread, Scope pattern guarantees its join.
    std::shared_ptr<InterruptibleThread> t_;

};

#endif