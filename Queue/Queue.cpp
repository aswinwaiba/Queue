// Queue.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Queue.h"
#include <chrono>
#include "SimpleLoopProcessor.h"
#include "SimpleEventProcessor.h"
#include <set>
#include <list>

std::mutex  global_logging_mutex_;


#define THREAD_SAFE_PRINT(message, num) \
{\
	std::lock_guard<std::mutex> lock(global_logging_mutex_);\
	std::cout << std::this_thread::get_id()  << " "  << __func__ << " " << message << " "#num" : " << num <<std::endl;\
}\

struct number_gen
{
	static std::size_t get_number()
	{
		std::lock_guard<std::mutex> lg(lock_);
		++number_;

		if (number_ >= limit_)
			cond_.notify_one();

		return number_;
	}

	static std::size_t peek()
	{
		std::lock_guard<std::mutex> lg(lock_);
		return number_;
	}

	static bool check_limit()
	{
		return !(number_ < limit_);
	}

	static void wait_for_limit()
	{
		std::unique_lock<std::mutex> lk(lock_);
		cond_.wait(lk, check_limit);
	}

	static std::size_t number_;
	static std::size_t limit_;
	static std::mutex lock_;
	static std::condition_variable cond_;
};

std::size_t number_gen::number_ = 0;
std::size_t number_gen::limit_ = 100;
std::mutex number_gen::lock_;
std::condition_variable number_gen::cond_;


//int main()
//{
//	//producer
//
//	auto cosume = [](std::size_t&& v) {
//		THREAD_SAFE_PRINT(" Consuming ", v);
//	};
//	SimpleEventProcessor<std::size_t> consumer(cosume);
//
//	auto pusher = [&consumer]() {
//		std::size_t num = number_gen::get_number();
//		THREAD_SAFE_PRINT(" Producing ", num);
//		consumer.push(num);
//	};
//	SimpleLoopProcessor<std::size_t> producer(pusher);
//
//	producer.start();
//	consumer.start();
//
//	number_gen::wait_for_limit();
//
//	producer.stop();
//	consumer.stop();
//
//	return 0;
//}


//TODO:
//1. Multiple consumer producer single thread.
//2. taskset and thread pinning


//is_specialization<vec,std::vector>::value 
//is_specialization<not_vec,std::vector>::value

template<typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type 
{
	//std::cout << __PRETTY_FUNCTION__ << std::endl;
};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> :std::true_type 
{
	//std::cout << __PRETTY_FUNCTION__ << std::endl;
};

int main()
{
	using vec = std::vector<int>;
	using not_vec = int;

	std::cout << is_specialization<vec, std::vector>::value << std::endl;
	std::cout << is_specialization<not_vec, std::vector>::value << std::endl;
	std::cout << is_specialization<std::list<int>, std::list>::value << std::endl;
	std::cout << is_specialization<vec, std::list>::value << std::endl;

	return 0;
}



//template <typename>
//struct is_vector :public std::false_type {};
//
//template<typename T, typename A>
//struct is_vector<std::vector<T, A> > : public std::true_type {};
//
//template <typename T>
//void rearrange_array(typename std::enable_if<is_vector<T>::value,T>::type& input1 )
//{
//	using v_iter = typename T::iterator;
//
//	auto z_pos = std::begin(input1);
//	auto n_pos = z_pos;
//
//	auto end = std::end(input1);
//	auto not_zero = [](int  i) -> bool { return i != 0; };
//
//	while (z_pos != end && n_pos != end) {
//		z_pos = std::find(z_pos, end, 0);
//		n_pos = std::find_if(z_pos, end, not_zero);
//		if (z_pos != end && n_pos != end)
//			std::iter_swap(z_pos, n_pos);
//	}
//}
//
//
//template <typename T>
//void try_rearrange_array(T& input) {
//	std::cout << is_vector<T>::value << std::endl;
//	if (is_vector<T>::value) {
//		std::cout << "Is a vector!" << std::endl;
//		rearrange_array<T>(input);
//	}
//	else {
//		std::cout << "Is not a vector!" << std::endl;
//	}
//}
//Program to take an array and re arrange it keeping order intact. zeros to back 
//
//int main()
//{
//	std::vector<int> input1 = { 1,2,4,0,0,3,0,7,8 };
//	std::vector<int> output1 = { 1,2,4,3,7,8,0,0,0 };
//	int size1 = static_cast<int>(input1.size());
//
//	std::vector<int> input2 = { 1, 2, 0, 0, 0, 3, 6 };
//	std::vector<int> output2 = { 1, 2, 3, 6, 0, 0, 0 };
//	int size2 = static_cast<int>(input2.size());
//
//	std::set<int,std::greater<int>> input3 = { 1,2,3,4,5,6,7 };
//	std::set<int> input4 = { 1,2,3,4,5,6,7 };
//
//	try_rearrange_array<std::vector<int>>(input1);
//	try_rearrange_array<std::vector<int>>(input2);
//	//try_rearrange_array<std::set<int, std::greater<int>>>(input3);
//	try_rearrange_array < std::set<int>>(input4);
//	
//	for (auto const &i : input1) {
//		std::cout << i <<"\t"; 
//	}
//	std::cout << std::endl;
//
//	std::cout << "Input1 and Output1 are " << (std::equal(std::begin(input1), std::end(input1), std::begin(input1))? "" : "not ") << "equal\n" ;
//
//	for (auto const &i : input2) {
//		std::cout << i << "\t";
//	}
//	std::cout << std::endl;
//
//	std::cout << "Input2 and Output2 are " << (std::equal(std::begin(input1), std::end(input1), std::begin(input1)) ? "" : "not " )<< "equal\n";
//
//	return 0;
//}