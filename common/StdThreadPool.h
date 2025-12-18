#pragma once

#if defined(PIXELPART_RUNTIME_MULTITHREADING) || defined(__INTELLISENSE__)

#include "ThreadPool.h"
#include <utility>
#include <vector>
#include <queue>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace pixelpart {
class StdThreadPool : public ThreadPool {
public:
	StdThreadPool(std::size_t threads);
	virtual ~StdThreadPool();

	virtual void enqueueTask(std::uint32_t taskId, Task task) override;
	virtual void wait(std::uint32_t taskId) override;

	virtual std::size_t threadCount() const override;

private:
	std::vector<std::thread> workers;
	std::queue<std::pair<std::uint32_t, Task>> tasks;
	std::unordered_set<std::uint32_t> runningTasks;

	std::mutex queueMutex;
	std::mutex stateMutex;
	std::condition_variable completionCondition;
	std::condition_variable workerCondition;
	bool stop = false;
};
}

#endif
