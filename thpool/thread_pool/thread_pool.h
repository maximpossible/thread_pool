/// @file thread_pool.h
/// @brief Contains ThreadPool class declaration.

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thpool/task_queue/task_queue.h>

#include <vector>
#include <memory>

namespace thread_pool
{

class ThreadObject;

/// @class ThreadPool
/// @brief Takes functions with their arguments, and distributes them between threads.
class ThreadPool
{
public:
    /// @brief Constructor of class ThreadPool.
    /// @details Run the number of worker threads available to the system.
    ThreadPool();

    /// @brief Add task to queue of tasks.
    /// @details Put task to queue. Task will be invoked in turn.
    /// @param[in] function Function, functor or lambda to be invoked.
    /// @param[in] args Arguments of function.
    template<typename T, typename... Args>
    void AddTask(T function, Args... args)
    {
        taskQueue->Push(std::forward<T>(function), std::forward<Args>(args)...);
    }

    /// @brief Destroy the thread pool.
    /// @details Stop all the free worker threads and marks busy
    /// worker threads, so they will stop after the task is completed.
    /// Can block the execution of the main thread until all tasks are completed.
    void DestroyThreadPool();


private:
    std::vector<std::shared_ptr<ThreadObject>> taskExecutors;   ///< Vector of pointer to worker threads.
    std::shared_ptr<std::condition_variable> newTaskCondition;  ///< Pointer to conditional variable, that
                                                                ///  will signal about new task.
    std::shared_ptr<std::mutex> newTaskLock;                    ///< Pointer to mutex for conditional variable.
    std::shared_ptr<TaskQueue> taskQueue;                       ///< Pointer to queue of tasks.
};

} // thread_pool

#endif // THREAD_POOL_H
