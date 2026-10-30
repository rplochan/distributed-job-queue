#include "../include/threadpool.h"
#include <stdexcept>

/* 
constructor - creates the worker threads.
enqueue() - enqueues the task in the queue.
worker-loop - helps in waiting and executing the task.
destructor - shuts down the pool and joins the thread.
*/

ThreadPool::ThreadPool(size_t threadcount):stop(false) {

    for(size_t i =0; i <threadcount; i++){
        workers.emplace_back([this](){
            // search for the tasks

            while(true){
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
 
                    condition.wait(lock, [this](){
                        return stop || !tasks.empty();
                    }); 

                    // no task.
                    if(stop && tasks.empty()){
                        return;
                    }

                    task = std::move(tasks.front());
                    tasks.pop();

                }

                task();
            }
            

        });
    }

}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
    }

    condition.notify_one();
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();

    for (std::thread& worker : workers) {
        worker.join();
    }
}

