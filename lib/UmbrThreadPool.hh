#ifndef UMBR_THREAD_POOL_GUARD
#define UMBR_THREAD_POOL_GUARD

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class UmbrThreadPool {
public:
    UmbrThreadPool(size_t numThreads) : stopFlag(false) {
        start(numThreads);
    }

    ~UmbrThreadPool() {
        stop();
    }

    template<typename Func, typename... Args>
    void execute(Func&& f, Args&&... args) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
        }
        condition.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(doneMutex);
        doneCond.wait(lock, [this]() {
            return tasks.empty() && activeTasks == 0;
        });
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;

    std::atomic<int> activeTasks{0};
    std::condition_variable doneCond;
    std::mutex doneMutex;

    void start(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this]() {
                            return stopFlag || !tasks.empty();
                        });

                        if (stopFlag && tasks.empty())
                            return;

                        task = std::move(tasks.front());
                        tasks.pop();
                        ++activeTasks;
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(doneMutex);
                        --activeTasks;
                        if (tasks.empty() && activeTasks == 0) {
                            doneCond.notify_all();
                        }
                    }
                }
            });
        }
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stopFlag = true;
        }

        condition.notify_all();
        for (auto& t : workers)
            t.join();
    }
};

#endif
