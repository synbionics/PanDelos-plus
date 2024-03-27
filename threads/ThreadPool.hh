#ifndef THREAD_POOL_INCLUDE_GUARD
#define THREAD_POOL_INCLUDE_GUARD 1


#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>


/**
 * @file ThreadPool.hh
 * @brief Definitions for the ThreadPool class.
 */

/**
 * @namespace threads
 * @brief Namespace containing definitions for threads related classes.
 */

namespace threads {

    /**
     * @class ThreadPool
     * @brief A simple thread pool implementation.
     *
     * This class provides a basic thread pool with the ability to execute tasks in parallel.
     */
    class ThreadPool {
        private:
            using thread_t = std::thread;
            using mutex_t = std::mutex;
        public:
            using task_t = std::function<void()>;
        private:
            using thread_ct = std::vector<thread_t>;
            using queue_t = std::queue<task_t>;

            size_t totalThread_;
            size_t tasksNumber_;
            std::condition_variable queueNotEmpty_;
            std::condition_variable workDone_;
            mutex_t workMutex_;
            mutex_t queueMutex_;
            thread_ct threads_;
            queue_t taskQueue_;
            bool shutdown_;

            /**
             * @brief The main loop executed by each thread in the pool.
             */
            inline void loop();
        public:

            inline void waitTasks();

            /**
             * @brief Checks if all tasks in the pool have been completed.
             * @return True if all tasks are completed, false otherwise.
             */
            inline bool tasksCompleted();
            /**
             * @brief Constructs a new ThreadPool object with the specified number of threads.
             * @param threadNumber The number of threads in the pool.
             */
            inline ThreadPool(size_t threadNumber);
            /**
             * @brief Constructs a new ThreadPool object with the number of threads equal to the hardware concurrency.
             */
            inline ThreadPool();

            /**
             * @brief Gets the total number of threads in the pool.
             * @return The total number of threads in the pool.
             */
            inline size_t getTotalThread() const {
                return totalThread_;
            }
            /**
             * @brief Starts the thread pool by creating and launching threads.
             */
            inline void start();
            /**
             * @brief Stops the thread pool by joining all threads.
             */
            inline void stop();
            /**
             * @brief Executes a task in the thread pool.
             * @param task The task to execute.
             */
            inline void execute(const task_t& task);

            /**
             * @brief Destroys the ThreadPool object and stops the thread pool.
             */
            inline ~ThreadPool();
    };

    inline bool
    ThreadPool::tasksCompleted() {
        {
            std::unique_lock<mutex_t> lock(queueMutex_);
            return tasksNumber_ == 0;
        }
    }

    inline ThreadPool::ThreadPool(size_t threadNumber)
    : totalThread_(threadNumber), tasksNumber_(0), shutdown_(false)
    {
    }

    inline void
    ThreadPool::loop() {
        while(true) {
            task_t task;
            {
                std::unique_lock<mutex_t> lock(queueMutex_);
                queueNotEmpty_.wait (
                    lock, [this] {
                        return !taskQueue_.empty() || shutdown_;
                    }
                );
                
                if(shutdown_)
                    break;
                
                task = taskQueue_.front();
                taskQueue_.pop();
            }

            task();
            {
                std::unique_lock<mutex_t> lock(queueMutex_);
                --tasksNumber_;
                if(tasksNumber_ == 0) {
                    std::unique_lock<mutex_t> lock(workMutex_);
                    workDone_.notify_all();
                }
            }
        }
    }
    
    inline
    ThreadPool::ThreadPool()
    : totalThread_(std::thread::hardware_concurrency()), tasksNumber_(0), shutdown_(false){
    }

    inline void
    ThreadPool::start() {
        for(size_t i = 0; i < totalThread_; ++i) 
            threads_.emplace_back(thread_t(&ThreadPool::loop, this));
    }
    
    inline void
    ThreadPool::stop() {
        {
            std::unique_lock<mutex_t> lock(queueMutex_);
            std::unique_lock<mutex_t> wlock(workMutex_);
            shutdown_ = true;
            queueNotEmpty_.notify_all();
            workDone_.notify_all();
        }
        
        for(auto i = threads_.begin(); i != threads_.end(); ++i)
            i->join();
        
        threads_.clear();
    }

    inline void
    ThreadPool::execute(const task_t& task) {
        {
            std::unique_lock<mutex_t> lock(queueMutex_);
            ++tasksNumber_;
            taskQueue_.push(
                task
            );
            queueNotEmpty_.notify_one();
        }
    }

    // inline void
    // ThreadPool::waitTasks() {
    //     std::unique_lock<mutex_t> lock(queueMutex_);
    //     if(tasksNumber_ != 0) {
    //         std::unique_lock<mutex_t> wLock(workMutex_);
    //         lock.unlock();

    //         workDone_.wait(
    //             wLock, [this] {
    //                 return shutdown_;
    //             }
    //         );
    //     }
    // }

    ThreadPool::~ThreadPool() {
        stop();
    }
}


#endif