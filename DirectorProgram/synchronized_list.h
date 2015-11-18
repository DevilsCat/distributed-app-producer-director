// synchronized_list.h -- Declares and implements a concurrent list with ability to shutdown and start-up.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao.
//
#ifndef SYNCHRONIZED_LIST_H
#define SYNCHRONIZED_LIST_H

#include "stdafx.h"
#include <list>
#include <condition_variable>
#include <mutex>
#include <memory>

/* Modify your implementation of the synchronized_list so that it has a flag to determine whether or not it has been shut 
 * down, and update the synchronization logic so that if it has been shut down a thread that would have waited for its 
 * condition to become true now simply returns from the method it is in. Add a shut_down method that 
 *  (1) acquires the lock, 
 *  (2) sets that flag 
 *  (3) and then calls notify_all (to wake up all waiting threads) before releasing the lock and returning.*/

template<typename T>
class synchronized_list {
private:
	std::list<T> list_;
	mutable std::mutex mut; // ex5
	std::condition_variable cond_consume;
	std::condition_variable cond_provide;
	size_t high_wm_, low_wm_;

	bool shutdown_f_; // studio active object
public:
	synchronized_list() : 
		high_wm_(0),low_wm_(0), shutdown_f_(false) 
	{}
	synchronized_list(int high_wm, int low_wm) : 
		high_wm_(high_wm), low_wm_(low_wm), shutdown_f_(false) 
	{}

	void shut_down() {
		std::lock_guard<std::mutex> lk(mut);
		shutdown_f_ = true;
		cond_consume.notify_all();
		cond_provide.notify_all();
	}

	// start up the synchronized list to make sure the pop behaivior
	void start_up() {
		std::lock_guard<std::mutex> lk(mut);
		shutdown_f_ = false;
	}

	void push_back(T& value) {
		std::unique_lock<std::mutex> lk(mut);
		if (!shutdown_f_ && high_wm_ && list_.size() > high_wm_) { // provider need wait for consuming
			cond_provide.wait(lk, [this]{ return list_.size() <= high_wm_ || shutdown_f_; });
		}
		list_.push_back(value);
		cond_consume.notify_all();
	}

	std::shared_ptr<T> pop_back() {
		std::unique_lock<std::mutex> lk(mut);
		if (!shutdown_f_ && list_.empty() || list_.size() < low_wm_) {
			cond_consume.wait(lk, [this]{ return !list_.empty() && list_.size() >= low_wm_ || shutdown_f_ ; });
		}
		if (shutdown_f_) { return std::shared_ptr<T>(); }
		std::shared_ptr<T> res(std::make_shared<T>(std::move(list_.back())));
		list_.pop_back();
		if (high_wm_) cond_provide.notify_all();
		return res;

	}

	void push_front(T&& value) {
		std::unique_lock<std::mutex> lk(mut);
		if (!shutdown_f_ && high_wm_ && list_.size() > high_wm_) { // provider need wait for consuming
			cond_provide.wait(lk, [this]{ return list_.size() <= high_wm_ || shutdown_f_; });
		}
		list_.push_front(std::move(value));
		cond_consume.notify_all();
	}

	std::shared_ptr<T> pop_front() {
		std::unique_lock<std::mutex> lk(mut);
		if (!shutdown_f_ && list_.empty() || list_.size() < low_wm_) {
			cond_consume.wait(lk, [this]{ return !list_.empty() && list_.size() >= low_wm_ || shutdown_f_; });
		}
		if (shutdown_f_) { return std::shared_ptr<T>(); }
		std::shared_ptr<T> res(std::make_shared<T>(list_.front()));
		list_.pop_front();
		if (high_wm_) cond_provide.notify_all();
		return res;
	}

	size_t size() const{
		std::lock_guard<std::mutex> lk(mut);
		return list_.size();
	}

	friend std::ostream& operator<< (std::ostream& os, const synchronized_list<T>& item) {
		std::copy(item.list_.begin(), item.list_.end(), std::ostream_iterator<T>(os, " "));
		return os;
	}
};

#endif