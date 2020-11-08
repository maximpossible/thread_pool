#include <thpool/thread_pool/thread_pool.h>
#include <thpool/thread_pool/src/thread_object.h>

namespace thread_pool
{

ThreadPool::ThreadPool()
{
    newTaskLock = std::make_shared<std::mutex>();
    newTaskCondition = std::make_shared<std::condition_variable>();
    taskQueue = std::make_shared<TaskQueue>(newTaskCondition, newTaskLock);
 
    auto usefullThreads = std::thread::hardware_concurrency();
    taskExecutors.reserve(usefullThreads);
    for(size_t i = 0; i < usefullThreads; ++i)
    {
        auto threadObject = std::make_shared<ThreadObject>(newTaskCondition, newTaskLock, taskQueue);
        taskExecutors.push_back(threadObject);
    }
}

void ThreadPool::DestroyThreadPool()
{
    for (auto& i :taskExecutors)
    {
        i->Disable();
    }
}

} // thread_pool
