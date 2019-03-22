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

	void start() {} //TODO: implement
	void stop(){} //TODO: implement

	void run() {} //TODO: implement

	template <typename U=T, typename std::enable_if<std::is_class<U>::value>::type* = nullptr >
	void push(T const& val) {}//TODO: implement

	template <typename U=T, typename std::enable_if<!std::is_class<U>::value>::type* =nullptr >
	void push(T val) {}//TODO: implement


private:
	QueueType q_;
	std::thread th_;
	std::mutex mutex_;
	bool running_;
	CallBackType callback_;
};
