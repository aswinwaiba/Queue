#pragma once

template <typename T, typename call_back_type = std::function<void (void)> >
class SimpleLoopProcessor
{
public:
	SimpleLoopProcessor(call_back_type const& call_back)
		: th_()
		, mutex_()
		, running_(false)
		, call_back_func_(call_back)
	{}

	SimpleLoopProcessor(const SimpleLoopProcessor& copy) = delete;
	~SimpleLoopProcessor()
	{
		running_ = false;
		if (th_.joinable())
			th_.join();
	}

	SimpleLoopProcessor(SimpleLoopProcessor&&) = default;

	void start() {
		std::lock_guard<std::mutex> lk(mutex_);
		if (running_)
			return;

		running_ = true;
		th_ = std::thread(&SimpleLoopProcessor::run, this);
	}

	void stop() {
		std::lock_guard<std::mutex> lk(mutex_);
		running_ = false;

		return;
	}

	void run() {
		while (is_running()) {
			call_back_func_();
		}
	}

	bool inline is_running() {
		std::lock_guard<std::mutex> lk(mutex_);
		return running_;
	}


private:
	std::thread th_;
	std::mutex mutex_;
	bool running_;
	call_back_type call_back_func_;
};