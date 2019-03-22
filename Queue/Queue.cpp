// Queue.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Queue.h"
#include <chrono>
#include "SimpleLoopProcessor.h"

std::mutex  global_logging_mutex_;

#define THREAD_SAFE_PRINT(message, num) \
{\
	std::lock_guard<std::mutex> lock(global_logging_mutex_);\
	std::cout << std::this_thread::get_id() << " "  << __func__ << " " << message << " "#num << num <<std::endl;\
}\

struct number_gen
{
	static std::size_t get_number()
	{
		std::lock_guard<std::mutex> lg(lock_);
		return ++number_;
	}

	static std::size_t peek()
	{
		std::lock_guard<std::mutex> lg(lock_);
		return number_;
	}

	static std::size_t number_;
	static std::mutex lock_;
};

std::size_t number_gen::number_ = 0;
std::mutex number_gen::lock_;

template <typename T>
struct number_pusher
{
	std::thread th_;
	threading_model::thread_safe_queue<T>& q_;
	bool running_; //TODO : change this to atomic
	
	number_pusher(number_pusher&& num_push) = default;

	number_pusher(threading_model::thread_safe_queue<T>& q)
		: th_()
		, q_(q)
		, running_(false)
	{}

	number_pusher(const number_pusher& num_push) = delete;

	~number_pusher()
	{
		running_ = false;
		if(th_.joinable())
			th_.join();
	}

	void run()
	{
		while (running_) {
			std::size_t num = number_gen::get_number();
			q_.push(num);
			THREAD_SAFE_PRINT("Pushing", num);
			std::this_thread::sleep_for(std::chrono::milliseconds(50) ) ;
		}
	}

	void start()
	{
		if (running_)
			return;

		running_ =  true;
		th_ = std::thread(&number_pusher<T>::run, this);
	}

	void stop()
	{
		running_ = false; // TODO: Change this to atomic
	}
};


template <typename T>
struct number_reader
{
	std::thread th_;
	threading_model::thread_safe_queue<T>& q_;
	bool running_;

	number_reader(threading_model::thread_safe_queue<T>& q)
		:th_()
		, q_(q)
		, running_(false)
	{}

	number_reader(number_reader&& nr) = default;

	~number_reader()
	{
		running_ = false;
		if (th_.joinable())
			th_.join();
	}

	void start()
	{
		if (running_)
			return;

		running_ = true;
		th_ = std::thread(&number_reader<T>::run, this);
	}

	void stop()
	{
		if (running_)
			running_ = false;
	}

	void run()
	{
		while (running_) {
			std::size_t num = q_.pop();

			THREAD_SAFE_PRINT("Popping", num);
		}
	}
};

int main()
{
	//SimpleLoopProcessor<std::size_t> processor([]() {
	//	std::cout << "Print " << std::endl;
	//}
	//);

	//processor.start();
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	//processor.stop();

	long i = 0;
	auto looper = [&i]() {
		std::cout << " i : " << ++i << std::endl;

	};

	SimpleLoopProcessor<std::string> processor(looper);

	processor.start();
	while (i < 10000) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	processor.stop();
	return 0;
	
	threading_model::thread_safe_queue<std::size_t> q;

	typedef number_pusher<std::size_t> size_t_pusher;

	typedef number_pusher<std::size_t> size_t_pusher;
	std::vector<size_t_pusher>  pusher_th_group;
	for (std::size_t i = 0; i < 3; ++i)
	{
		pusher_th_group.push_back(size_t_pusher(q));
	}

	using size_t_reader = number_reader<std::size_t>;
	std::vector<size_t_reader> reader_th_group;
	for (std::size_t i = 0; i < 2; ++i)
	{
		reader_th_group.push_back(size_t_reader(q));
	}

	std::for_each(std::begin(pusher_th_group), std::end(pusher_th_group), std::mem_fn(&size_t_pusher::start));
	std::for_each(std::begin(reader_th_group), std::end(reader_th_group), std::mem_fn(&size_t_reader::start));

	//TODO: Use cond variable to stop the thread
	while (number_gen::peek() < 100) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	std::for_each(std::begin(pusher_th_group), std::end(pusher_th_group), std::mem_fn(&size_t_pusher::stop));

	while (!q.empty()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	std::for_each(std::begin(pusher_th_group), std::end(pusher_th_group), std::mem_fn(&size_t_pusher::stop));

    return 0;
}

