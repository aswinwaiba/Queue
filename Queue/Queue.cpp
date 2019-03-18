// Queue.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Queue.h"


struct number_gen
{
	static std::size_t get_number()
	{
		std::lock_guard<std::mutex> lg(lock_);
		return ++number_;
	}

	static std::size_t number_;
	static std::mutex lock_;
};

std::size_t number_gen::number_ = 0;
std::mutex number_gen::lock_;


int main()
{
	threading_model::thread_safe_queue<std::size_t> q;

	//make 3 threads and push into queue
	//make 1 thread and consume data

    return 0;
}

