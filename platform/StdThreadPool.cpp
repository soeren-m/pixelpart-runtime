#include "StdThreadPool.h"
#include <stdexcept>

#if defined(PIXELPART_RUNTIME_MULTITHREADING) || defined(__INTELLISENSE__)

namespace pixelpart {
StdThreadPool::StdThreadPool(std::size_t threads) {
	for(std::size_t threadIndex = 0; threadIndex < threads; threadIndex++) {
		workers.emplace_back([this]() {
			while(true) {
				std::uint32_t taskId = 0;
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
StdThreadPool::~StdThreadPool() {
	{
		std::unique_lock<std::mutex> lock(queueMutex);

		stop = true;
		workerCondition.notify_all();
	}

	for(std::thread& worker : workers) {
		worker.join();
	}
}

void StdThreadPool::enqueueTask(std::uint32_t taskId, Task task) {
	{
		std::unique_lock<std::mutex> lock(stateMutex);

		if(runningTasks.count(taskId) != 0) {
			throw std::runtime_error("Thread pool already contains task with given id");
		}

		runningTasks.insert(taskId);
	}

	{
		std::unique_lock<std::mutex> lock(queueMutex);

		tasks.emplace(std::make_pair(taskId, task));
		workerCondition.notify_one();
	}
}
void StdThreadPool::wait(std::uint32_t taskId) {
	std::unique_lock<std::mutex> lock(stateMutex);

	completionCondition.wait(lock, [this, taskId]() {
		return runningTasks.count(taskId) == 0;
	});
}

std::size_t StdThreadPool::threadCount() const {
	return workers.size();
}
}

#endif
