#include <thpool/thread_pool/src/thread_object.h>

#include <iostream>

namespace thread_pool
{

ThreadObject::ThreadObject(std::shared_ptr<std::condition_variable>& newTaskCondition, std::shared_ptr<std::mutex>& newTaskLock, 
    std::shared_ptr<TaskQueue>& taskQueue)
    : exceptions{}, taskQueue{taskQueue}, newTaskCondition{newTaskCondition}, newTaskLock{newTaskLock}, isWorkable{true}
{
    thread = std::thread(&ThreadObject::TaskExecutor, this);
}

ThreadObject::ThreadObject(ThreadObject&& obj)
{
    taskQueue = std::move(obj.taskQueue);
    thread = std::move(obj.thread);
    newTaskCondition = std::move(obj.newTaskCondition);
    newTaskLock = std::move(newTaskLock);
    isWorkable = std::move(obj.isWorkable);
    exceptions = std::move(obj.exceptions);
}

void ThreadObject::Disable()
{
    isWorkable = false;
    newTaskCondition->notify_all();
}

ThreadObject::~ThreadObject()
{
    if (thread.joinable())
    {
        thread.join();
    }
}

std::vector<std::exception_ptr> ThreadObject::GetExceptions() const noexcept
{
    return exceptions;
}

void ThreadObject::TaskExecutor()
{
    while(isWorkable)
    {
        {
            std::unique_lock<std::mutex> lock(*newTaskLock);
            newTaskCondition->wait(lock, [&]()
            {
                return taskQueue->Size() || !isWorkable;
            });
        }
        try
        {
            if (taskQueue->Size())
            {
                auto function = taskQueue->GetFunction();
                function();
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << "Exception in work-thread: " << e.what() << std::endl;
            exceptions.push_back(std::current_exception());
        }
    }
}

} // thread_pool
