#include <thpool/task_queue/task_queue.h>

namespace thread_pool
{

TaskQueue::TaskQueue(std::shared_ptr<std::condition_variable>& newTaskCondition, std::shared_ptr<std::mutex> newTaskLock)
        : queueLock{}, taskQueue(), newTaskCondition{newTaskCondition}, newTaskLock{newTaskLock} {}

std::function<void()> TaskQueue::GetFunction()
{
    std::lock_guard<std::mutex> lock(queueLock);
    if (!taskQueue.empty())
    {
        auto task = taskQueue.front();
        taskQueue.pop();
        return task;
    }

    throw std::out_of_range("Queue is empty\n");
}

size_t TaskQueue::Size()
{
    return taskQueue.size();
}

} // thread_pool
