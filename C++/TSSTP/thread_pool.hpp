#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

struct Thread_pool {
	Thread_pool(unsigned int threads = std::thread::hardware_concurrency())
		: workers(threads) {
		for (auto &thread : workers) {
			thread = std::thread{[this] {
				for (;;) {
					std::function<void()> work;
					{
						std::unique_lock l{worker_queue_mutex};
						condition_variable.wait(l, [this] { return not work_queue.empty(); });
						work = std::move(work_queue.front());
						work_queue.pop_front();
					}
					auto work_pointer = work.target<void (*)()>();
					if (work_pointer && *work_pointer == &Thread_pool::quit) {
						return;
					}
					work();
				}
			}};
		}
	}
	Thread_pool(const Thread_pool &) = delete;
	Thread_pool &operator=(const Thread_pool &) = delete;
	~Thread_pool() {
		{
			std::unique_lock l(worker_queue_mutex);
			for (const auto &thread : workers) {
				work_queue.emplace_back(&quit);
			}
		}
		condition_variable.notify_all();
		for (auto &thread : workers) {
			thread.join();
		}
	}

	void push(std::function<void()> f) {
		std::unique_lock l(worker_queue_mutex);
		work_queue.push_back(std::move(f));
		condition_variable.notify_one();
	}

	private:
	static void quit() {}
	std::mutex worker_queue_mutex;
	std::deque<std::function<void()>> work_queue;
	std::vector<std::thread> workers;
	std::condition_variable condition_variable;
};