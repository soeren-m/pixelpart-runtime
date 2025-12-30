#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <future>
#include <type_traits>

namespace pixelpart {
class ThreadPool {
public:
	using Task = std::function<void()>;

	virtual ~ThreadPool() = default;

	template <typename F, typename... Args>
	void enqueue(std::uint32_t taskId, F&& f, Args&&... args) {
		auto task = std::make_shared<std::packaged_task<typename std::invoke_result<F, Args...>::type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		enqueueTask(taskId,
			[task]() {
				(*task)();
			});
	}

	virtual void enqueueTask(std::uint32_t taskId, Task task) = 0;
	virtual void wait(std::uint32_t taskId) = 0;

	virtual std::size_t threadCount() const = 0;
};
}
