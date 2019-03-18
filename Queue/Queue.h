#pragma once
#include <deque>
#include <queue>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>


namespace threading_model {

template <typename T>
class thread_safe_queue
{
public:
	thread_safe_queue() = default;
	//stop copy
	thread_safe_queue(const thread_safe_queue& tsq) = delete;

	template <typename U = T>
	void push(typename std::enable_if<std::is_class<U>::value, U>::type const& ref_elem);

	template <typename U = T>
	void push(typename std::enable_if < !std::is_class<U>::value, U >::type copy_elem);

	T pop();

	void pop(T& swap_value);

	std::size_t size();

	bool empty();

private:
	std::queue<T> queue_;
	std::mutex lock_;
	std::condition_variable cv_;
};


template<typename T>
template<typename U >
void thread_safe_queue<T>::push(typename std::enable_if< std::is_class<U>::value, U>::type const& ref_elem)
{
	{
		std::lock_guard<std::mutex> guard(lock_);
		queue_.push_back(ref_elem);
	}
	cv_.notify_one();
}

template<typename T>
template<typename U >
void thread_safe_queue<T>::push(typename std::enable_if< !std::is_class<U>::value, U>::type copy_elem)
{
	{
		std::lock_guard<std::mutex> guard(lock_);
		//queue.push_back
		queue_.emplace(copy_elem);
	}
	cv_.notify_one();
}

template <typename T>
T thread_safe_queue<T>::pop()
{
	std::unique_lock<std::mutex> lk(lock_);
	cv_.wait(lk, []() {return empty(); });

	T front = queue_.front();
	queue_.pop();

	return std::move(front);
}
//TODO: If some threads are waiting here and I have some data
// however if the queue object goes out of scope
// how should be notify the waiting threads.
template <typename T>
void thread_safe_queue<T>::pop(T& return_ref)
{
	std::unique_lock<std::mutex> lk(lock_);
	cv_.wait(lk, []() {return empty(); });

	return_ref = queue_.front();
	queue_.pop();

	return;
}

template <typename T>
bool thread_safe_queue<T>::empty()
{
	std::lock_guard<std::mutex> guard(lock_);
	return queue_.empty();
}

template < typename T>
std::size_t thread_safe_queue<T>::size()
{
	std::lock_guard<std::mutex> guard(lock_);
	queue_.size();
}

};//of threading_model