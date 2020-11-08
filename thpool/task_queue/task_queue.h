/// @file task_queue.h
/// @brief Contains TaskQueue class declaration.
#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <memory>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <stdexcept>

namespace thread_pool
{

/// @class TaskQueue
/// @brief Class stores functions with their arguments. 
class TaskQueue
{
public:
    /// @brief Constructor of TaskQueue class.
    /// @details Construct TaskQueue obeject by initializing fields and saving pointers
    /// to conditional variable and mutex for it.
    /// @param[in] newTaskCondition Pointer to conditional variable, that will signal about new task.
    /// @param[in,out] newTaskLock Pointer to mutex for conditional variable.
    TaskQueue(std::shared_ptr<std::condition_variable>& newTaskCondition, 
        std::shared_ptr<std::mutex> newTaskLock);

    /// @brief Add task to queue of tasks.
    /// @details Put task to queue. Task will be invoked in turn.
    /// @param[in] function Function, functor or lambda to be invoked.
    /// @param[in] args Arguments of function.
    template<typename T, typename... Args>
    void Push(T function, Args... args)
    {
        std::lock_guard<std::mutex> lock(queueLock);
        auto bindedFunction = std::bind(std::forward<T>(function), std::forward<Args>(args)...);
        taskQueue.push(std::move(bindedFunction));
        newTaskCondition->notify_one();
    }

    /// @brief Return function from queue.
    /// @details Return the function that was put into the queue first.
    /// @return std::function object, that stores the function and its arguments.
    /// @throws std::out_of_range exception, if queue is empty.
    std::function<void()> GetFunction();
    
    /// @brief Get size of queue.
    /// @return Count of functions in queue.
    size_t Size();


private:
    std::mutex queueLock;                                       ///< Mutex for locking queue from editing from different threads.
    std::queue<std::function<void()>> taskQueue;                ///< Queue of functions.
    std::shared_ptr<std::condition_variable> newTaskCondition;  ///< Pointer to conditional variable, that
                                                                ///  will signal about new task.
    std::shared_ptr<std::mutex> newTaskLock;                    ///< Pointer to mutex for conditional variable.
};

} // thread_pool

#endif // TASK_QUEUE_H
