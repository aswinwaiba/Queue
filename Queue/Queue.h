#pragma once
#include <deque>
#include <queue>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <functional>

template <typename T>
class thread_safe_queue
{
public:
	T pop();
	
	template <typename U=T>
	void push(std::enable_if<std::is_class<U>::value,T>::type const& ref_elem);

	template <typenaem U=T>
	void push(std::enable_if < !std::is_class<U>::value, T >::type copy_elem);

	std::size_t count();

	bool empty();


private:
	std::queue<T> queue_;
	std::mutex lock_;
	std::conditional_variable cv_;
};
