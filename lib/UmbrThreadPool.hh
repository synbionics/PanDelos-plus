#ifndef UMBR_THREAD_POOL_HH
#define UMBR_THREAD_POOL_HH

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cstddef>

class UmbrThreadPool {

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex mutex_;
    std::condition_variable cvWork_;
    std::condition_variable cvDone_;
    bool stopFlag_ = false;
    size_t activeTasks_ = 0;

    void start(size_t n) {
        workers_.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cvWork_.wait(lock, [this] {
                            return stopFlag_ || !tasks_.empty();
                        });

                        if (stopFlag_ && tasks_.empty())
                            return;

                        task = std::move(tasks_.front());
                        tasks_.pop();
                        ++activeTasks_;
                    }

                    try {
                        task();
                    } catch (...) {
                        // TODO gestire eccezione (il task del thread ha lanciato eccezione)
                    }

                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        --activeTasks_;
                        if (tasks_.empty() && activeTasks_ == 0)
                            cvDone_.notify_all();
                    }
                }
            });
        }
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stopFlag_ = true;
        }
        cvWork_.notify_all();
        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
    }

public:
    explicit UmbrThreadPool(size_t numThreads) { start(numThreads); }
    ~UmbrThreadPool() { stop(); }

    template<typename Func, typename... Args>
    bool execute(Func&& f, Args&&... args) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (stopFlag_) return false;
            tasks_.emplace(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
        }
        cvWork_.notify_one();
        return true;
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cvDone_.wait(lock, [this] {
            return tasks_.empty() && activeTasks_ == 0;
        });
    }
};

#endif