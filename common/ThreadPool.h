#pragma once

#ifdef PIXELPART_MULTITHREADING

#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <queue>
#include <unordered_set>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

namespace pixelpart {
class ThreadPool {
public:
	using Task = std::function<void()>;

	ThreadPool(std::size_t threads) {
		for(std::size_t i = 0u; i < threads; i++) {
			workers.emplace_back([this]() {
				while(true) {
					uint32_t taskId = 0u;
					Task task;

					{
						std::unique_lock<std::mutex> lock(queueMutex);

						workerCondition.wait(lock, [this] {
							return stop || !tasks.empty();
						});

						if(stop && tasks.empty()) {
							return;
						}

						taskId = tasks.front().first;
						task = std::move(tasks.front().second);
						tasks.pop();
					}

					task();

					{
						std::unique_lock<std::mutex> lock(stateMutex);

						runningTasks.erase(taskId);
						completionCondition.notify_all();
					}
				}
			});
		}
	}
	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);

			stop = true;
			workerCondition.notify_all();
		}

		for(std::thread& worker : workers) {
			worker.join();
		}
	}

	template<typename F, typename... Args>
	void enqueue(uint32_t taskId, F&& f, Args&&... args) {
		auto task = std::make_shared<std::packaged_task<typename std::result_of<F(Args...)>::type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		{
			std::unique_lock<std::mutex> lock(stateMutex);

			runningTasks.insert(taskId);
		}

		{
			std::unique_lock<std::mutex> lock(queueMutex);

			tasks.emplace(std::make_pair(
				taskId,
				[task]() {
					(*task)();
				}));

			workerCondition.notify_one();
		}
	}

	void wait(uint32_t taskId) {
		std::unique_lock<std::mutex> lock(stateMutex);

		completionCondition.wait(lock, [this, taskId]() {
			return runningTasks.count(taskId) == 0u;
		});
	}

	std::size_t threadCount() const {
		return workers.size();
	}

private:
	std::vector<std::thread> workers;
	std::queue<std::pair<uint32_t, Task>> tasks;
	std::unordered_set<uint32_t> runningTasks;

	std::mutex queueMutex;
	std::mutex stateMutex;
	std::condition_variable completionCondition;
	std::condition_variable workerCondition;
	bool stop = false;
};
}

#endif