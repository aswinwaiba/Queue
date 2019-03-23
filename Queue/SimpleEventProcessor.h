#pragma once

#include "Queue.h"

template <typename T, typename QueueType = threading_model::thread_safe_queue<T> >
class SimpleEventProcessor 
{
public:
	using CallBackType = std::function<void (T&&)>;
	SimpleEventProcessor(CallBackType const& cb)
		: q_()
		, th_()
		, mutex_()
		, running_(false)
		, callback_(cb)
	{}

	SimpleEventProcessor(SimpleEventProcessor const& sep) = delete;
	~SimpleEventProcessor()
	{
		std::lock_guard<std::mutex> lk(mutex_);
		running_ = false;
		if (th_.joinable())
			th_.join();
	}

	SimpleEventProcessor(SimpleEventProcessor&& msep) = default;

	void start() 
	{
		std::lock_guard<std::mutex> lk(mutex_);
		running_ = true;
		th_ = std::thread(&SimpleEventProcessor::run, this);
	} 

	void stop()
	{
		std::lock_guard<std::mutex> lk(mutex_);
		//Clean queue should be configurable
		while (!q_.empty()) {
			T&& val = q_.pop();
			callback_(std::move(val));
		}
		running_ = false;
	} 
	
	inline bool is_running()
	{
		std::lock_guard<std::mutex> lk(mutex_);
		return running_;
	}
	void run() 
	{
		while (is_running() ) {
			T&& val = q_.pop();
			callback_(std::move(val));
		}
	}

	template <typename U=T, typename std::enable_if<std::is_class<U>::value>::type* = nullptr >
	void push(T const& val) {
		q_.push(val);
	}

	template <typename U=T, typename std::enable_if<!std::is_class<U>::value>::type* =nullptr >
	void push(T val) {
		q_.push(val);
	}


private:
	QueueType q_;
	std::thread th_;
	std::mutex mutex_;
	bool running_;
	CallBackType callback_;
};
