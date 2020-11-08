/// @file thread_object.h
/// @brief Contains ThreadObject class declaration.
#ifndef THREAD_OBJECT_H
#define THREAD_OBJECT_H

#include <thpool/task_queue/task_queue.h>

#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread> 

namespace thread_pool
{

/// @class ThreadObject
/// @brief Stores functions with their arguments. 
class ThreadObject
{
public:
    /// @brief Constructor of class ThreadObject.
    /// @details Run thread with task execution handler.
    /// @param[in] newTaskCondition Pointer to conditional variable, that will signal about new task.
    /// @param[in,out] newTaskLock Pointer to mutex for conditional variable.
    /// @param[in,out] taskQueue Pointer to queue of tasks
    ThreadObject(std::shared_ptr<std::condition_variable>& newTaskCondition, 
        std::shared_ptr<std::mutex>& newTaskLock, std::shared_ptr<TaskQueue>& taskQueue);

    /// @brief Move-constructor of class ThreadObject.
    /// @details Move all the fields of obj into fields of current object.
    /// @param[in] obj R-value refference to object of class ThreadObject to be moved.
    ThreadObject(ThreadObject&& obj);

    ThreadObject(ThreadObject& obj) = delete;

    /// @brief Disable the execution handler.
    /// @details Stop the execution of tasks, if there are no active tasks.
    /// Otherwise, wait for end of last task execution.
    void Disable();

    /// @brief Return exceptions, thrown while executing tasks.
    /// @details Return exeptions, that were thrown while executing tasks. May be empty.
    /// @return Vector of pointers to thrown exceptions. 
    std::vector<std::exception_ptr> GetExceptions() const noexcept;

    /// @brief Destructor of class ThreadObject.
    /// @details Disable thread with execution handler.
    /// Can block main thread until the last task will be completed.
    ~ThreadObject();


private:

    /// @brief Execution handler.
    /// @details Execute task, poped from TaskQueue.
    void TaskExecutor();


private:
    std::thread thread;                                         ///< Thread which store execution handler.
    std::vector<std::exception_ptr> exceptions;                 ///< Vector of pointers of thrown exceptions.
    std::shared_ptr<TaskQueue> taskQueue;                       ///< Pointer to queue of tasks.
    std::shared_ptr<std::condition_variable> newTaskCondition;  ///< Pointer to conditional variable, that 
                                                                ///< will signal about new task.
    std::shared_ptr<std::mutex> newTaskLock;                    ///< Pointer to mutex for conditional variable.
    bool isWorkable;                                            ///< Variable, responsible for the operation of 
                                                                ///< the execution handler.
};

} // thread_pool

#endif // THREAD_OBJECT_H
