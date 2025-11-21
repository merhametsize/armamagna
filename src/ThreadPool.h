#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable> //For std::condition_variable
#include <functional>         //For std::function
#include <stop_token>         //For std::stop_token
#include <atomic>             //For nuclear energy☢️
#include <vector>             //For std::vector
#include <thread>             //For std::jthread
#include <mutex>              //For std::mutex
#include <queue>              //For std::queue

class ThreadPool 
{
public:
    explicit ThreadPool(int numThreads);
    ~ThreadPool();

    // Enqueue a task
    void enqueue(std::function<void()> task);

private:
    // Worker threads
    std::vector<std::jthread> workers;

    // Task queue
    std::queue<std::function<void()>> queue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    // Stop flag
    std::atomic<bool> stopping{false};

    // Worker loop
    void workerLoop();
};

#endif

