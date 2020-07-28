#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread> 
#include <condition_variable>
#include <memory>
#include <functional>
#include <queue>
#include <iostream>

/*
  BUGS:
    1. Если будет вызываться конструктор перемещения для ThreadObject, то все ломается.
        Например, если заранее не выделить память вектору
*/

struct TaskQueue
{
    TaskQueue(std::shared_ptr<std::condition_variable>& newTaskCondition, std::shared_ptr<std::mutex> newTaskLock)
        : _queueLock{}, _taskQueue(), _newTaskCondition{newTaskCondition}, _newTaskLock{newTaskLock} {}

    template<typename T, typename... Args>
    void Push(T func, Args... args)
    {
        std::lock_guard<std::mutex> lock(_queueLock);
        auto bindedFunction = std::bind(std::forward<T>(func), std::forward<Args>(args)...);
        _taskQueue.push(std::move(bindedFunction));
        _newTaskCondition->notify_one();
    }

    std::function<void()> GetFunction()
    {
        std::lock_guard<std::mutex> lock(_queueLock);
        if (!_taskQueue.empty())
        {
            auto task = _taskQueue.front();
            _taskQueue.pop();
            return task;
        }

        throw std::system_error();
    }

    size_t Size()
    {
        return _taskQueue.size();
    }

    std::mutex _queueLock;
    std::queue<std::function<void()>> _taskQueue;
    std::shared_ptr<std::condition_variable> _newTaskCondition;
    std::shared_ptr<std::mutex> _newTaskLock;
};

struct ThreadObject
{
    ThreadObject(std::shared_ptr<std::condition_variable>& newTaskCondition, std::shared_ptr<std::mutex>& newTaskLock, 
        std::shared_ptr<TaskQueue>& taskQueue, size_t id)
        : _taskQueue{taskQueue}, _newTaskCondition{newTaskCondition}, _newTaskLock{newTaskLock}, _threadId{id}, _isBusy{false}, _workable{true}
    {
        _thread = std::thread(&ThreadObject::TaskExecutor, this);
    }

    ThreadObject(ThreadObject& obj) = delete;

    ThreadObject(ThreadObject&& obj)
    {
        _taskQueue = std::move(obj._taskQueue);
        _thread = std::move(obj._thread);
        _newTaskCondition = std::move(obj._newTaskCondition);
        _newTaskLock = std::move(_newTaskLock);
        _threadId = std::move(obj._threadId);
        _isBusy = std::move(obj._isBusy);
        _workable = std::move(obj._workable);
    }

    void Disable()
    {
        _workable = false;
        _newTaskCondition->notify_all();
    }

    ~ThreadObject()
    {
        if (_thread.joinable())
        {
            _thread.join();
        }
    }

    void TaskExecutor()
    {
        while(_workable)
        {
            {
                std::unique_lock<std::mutex> lock(*_newTaskLock);
                _newTaskCondition->wait(lock, [&]()
                {
                    return _taskQueue->Size() || !_workable;
                });
            }
            try
            {
                if (this->_taskQueue->Size() > 0)
                {
                    auto function = _taskQueue->GetFunction();
                    function();
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << "Queue is empty" << '\n';
            }
        }
    }

    std::shared_ptr<TaskQueue> _taskQueue;
    std::thread _thread;
    std::shared_ptr<std::condition_variable> _newTaskCondition;
    std::shared_ptr<std::mutex> _newTaskLock;
    std::mutex _mtx;
    size_t _threadId;
    bool _isBusy;
    bool _workable;
};

class ThreadPool
{
public:
    ThreadPool()
    {
        _newTaskLock = std::make_shared<std::mutex>();
        _newTaskCondition = std::make_shared<std::condition_variable>();
        _taskQueue = std::make_shared<TaskQueue>(_newTaskCondition, _newTaskLock);
 
        auto usefullThreads = std::thread::hardware_concurrency();
        _taskExecutors.reserve(usefullThreads);
        for(auto i = 0; i < usefullThreads; ++i)
        {
            _taskExecutors.emplace_back(_newTaskCondition, _newTaskLock, _taskQueue, i);
        }
    }

    template<typename T, typename... Args>
    void AddTask(T function, Args... args)
    {
        _taskQueue->Push(std::forward<T>(function), std::forward<Args>(args)...);
    }

    void DestroyThreadPool()
    {
        for (auto& i : _taskExecutors)
        {
            i.Disable();
        }
    }

    std::shared_ptr<std::condition_variable> _newTaskCondition;
    std::shared_ptr<std::mutex> _newTaskLock;
    std::shared_ptr<TaskQueue> _taskQueue;
    std::vector<ThreadObject> _taskExecutors;
};

#endif
