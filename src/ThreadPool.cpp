#include <iostream>

#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads)
{
    for (int i = 0; i < numThreads; i++) 
    {
        workers.emplace_back([this] { workerLoop(); });
    }
}

ThreadPool::~ThreadPool()
{
    // Set stopping flag and wake all threads
    stopping = true;
    queueCV.notify_all();

    //JThreads are automatically joined here through RAII
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::scoped_lock lock(queueMutex);
        queue.push(std::move(task));
    }
    queueCV.notify_one();
}

void ThreadPool::workerLoop()
{
    while (true)
     {
        std::function<void()> task;

        {
            std::unique_lock lock(queueMutex);

            // Wait for a task or shutdown
            queueCV.wait(lock, [this]{ return stopping || !queue.empty(); });

            if (stopping && queue.empty())
                return; // exit thread

            task = std::move(queue.front());
            queue.pop();
        }

        try {task();}
        catch (const std::exception &e) {std::cerr << "[ThreadPool] task threw exception: " << e.what() << "\n";}
        catch (...)                     {std::cerr << "[ThreadPool] task threw unknown exception\n";}
    }
}
