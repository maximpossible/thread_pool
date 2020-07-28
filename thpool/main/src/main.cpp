#include <thpool/thread_pool/thread_pool.h>
#include <iostream>
#include <chrono>

void SayHelloHere()
{
    std::cout << "Hello there!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void SayHelloHereSum(int a, int b)
{
    std::cout << "Hello here sum: " << a + b << std::endl;
}

int main(int argc, char** argv)
{
    ThreadPool pool;

    pool.AddTask(SayHelloHere);
    pool.AddTask(SayHelloHere);
    pool.AddTask(SayHelloHere);
    pool.AddTask(SayHelloHereSum, 5, 5);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    pool.DestroyThreadPool();
    
    return 0;
}
