#ifndef THPOOL_H
#define THPOOL_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <vector>
#include <memory>
#include <chrono>

class Tpool {
	using Task = std::function<void(void)>;
public:
	Tpool(std::size_t sz): sz(sz)
	{
		start();
	}

	~Tpool()
	{
		stop();
	}

	template<typename T, typename ... Args>
	auto enq(T&& func, Args&& ... args) -> std::future<decltype(func(std::forward<Args>(args)...))>
	{
		auto ptask = std::make_shared<std::packaged_task<decltype(func(std::forward<Args>(args)...))(Args...)>>(std::forward<T>(func));
		{
			std::unique_lock<std::mutex> lock(mu);
			q.emplace([=]() mutable {
					(*ptask)(std::forward<Args>(args)...);
					});
		}
		cv.notify_one();
		return ptask->get_future();
	}

private:

	void start()
	{
		workers.reserve(sz);
		for (int i = 0; i < sz; i++)
		{
			workers.emplace_back([&]() {
				while (true) {
					Task tsk;
					{
						std::unique_lock<std::mutex> lock(mu);
						while (q.empty() && !pstop)
							cv.wait(lock);
						if (q.empty() && pstop)
							break;
						tsk = std::move(q.front());
						q.pop();
					}
					tsk();
				}
			});
		}
	}

	void stop() noexcept
	{
		{
			std::unique_lock<std::mutex> lock(mu);
			pstop = true;
		}
		cv.notify_all();
		for (auto &w : workers)
			w.join();
		std::cout << "all joined\n";
	}

	std::queue<Task> q;
	std::vector<std::thread> workers;

	std::mutex mu;
	std::condition_variable cv;

	bool pstop = false;
	std::size_t sz;
};

#endif
