#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <mutex>

#include "../include/job-parser.h"
#include "../include/processor.h"
#include "../include/redis-client.h"
#include "../include/postgres-client.h"
#include "../include/threadpool.h"

int main() {

    RedisClient redis;   // communicates with valkey
    JobParser parser;    // converts the raw messages into structured jobs.
    JobProcessor processor;
    PostgresClient postgres; // communicates with the database.

    ThreadPool pool(4); // creates 4 threads to wait for the tasks

    // important
    /*
        conceptually, the constructor starts with 4 threads, and wait for the tasks. when the task (callable) whis is
        enqued in the internal pool's queue. An available thread picks it and ezecute it.

        Four threads allow up to four submitted tasks to execute concurrently.
        They do not guarantee that every task runs simultaneously, because tasks may wait in the queue.
     */

    // shared resources...
    std::mutex dbMutex; // protects access to shared postgres object.
    std::mutex outputMutex;


    std::cout << "Worker started with 4 threads...\n";

    while (true) {

        try {

            // 1. Get a job from Redis
            std::string data = redis.getJob();

            // 2. No job available
            if (data.empty()) {
                //this sleep affects the main thread, not the other 4 worker threads
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(500)
                );

                continue;
            }

            // 3. Convert string → Job
            Job job = parser.parse(data);

            {
                std::lock_guard<std::mutex> lock(outputMutex);
                std::cout << "Received job: " << data << "\n";

            }

            /*
                - job — copied into the lambda. Each submitted task keeps its own copy of that job.
                - &postgres — refers to the one shared PostgreSQL client.
                - &dbMutex — refers to the mutex shared by all tasks that access that client.
                - &outputMutex — refers to the mutex shared by the threads that print messages.

                main thread can fetch the next job and change its own local job variable without
                changing the copy held by an already-submitted task.

             */

            pool.enqueue([job, &postgres, &dbMutex, &outputMutex](){
              try {
                    JobProcessor processor;
                //This protects the shared database connection from concurrent calls.
                    {
                        std::lock_guard<std::mutex> lock(dbMutex);
                        postgres.updateStatus(job.id, "processing");

                    }


                    if (job.type == "hash") {
                        // Execute the hash job.
                        std::string result = processor.processHash(job);

                        // Save the text result and mark the job completed.
                        {
                            std::lock_guard<std::mutex> lock(dbMutex);

                            postgres.updateTextResult(job.id, result);
                            postgres.updateStatus(job.id, "completed");
                        }

                        // Print the result.
                        {
                            std::lock_guard<std::mutex> lock(outputMutex);

                            std::cout << "Job " << job.id
                                      << " completed. SHA-256: "
                                      << result << '\n';
                        }
                    }
                    else {
                        // Execute an integer-result job.
                        int result = processor.process(job);

                        // Save the integer result and mark the job completed.
                        {
                            std::lock_guard<std::mutex> lock(dbMutex);

                            postgres.updateResult(job.id, result);
                            postgres.updateStatus(job.id, "completed");
                        }

                        // Print the result.
                        {
                            std::lock_guard<std::mutex> lock(outputMutex);

                            std::cout << "Job " << job.id
                                      << " completed. Result: "
                                      << result << '\n';
                        }
                    }
              }
              catch (const std::exception& e){
                {
                  std::lock_guard<std::mutex> lock(outputMutex);
                  std::cerr << "Job" << job.id << " failed: " << e.what() << '\n';
                }

                try {
                        std::lock_guard<std::mutex> lock(dbMutex);
                        postgres.updateStatus(job.id, "failed");
                    }
                catch (const std::exception& dbError) {
                        std::lock_guard<std::mutex> lock(outputMutex);
                        std::cerr << "Could not record failure for job "
                        << job.id << ": "
                        << dbError.what() << '\n';
                 }
              }

            });
        }

        catch (const std::exception& e) {

            {
                std::lock_guard<std::mutex> lock(outputMutex);
                std::cerr << "Worker error: " << e.what() << '\n';
            }

            std::this_thread::sleep_for(
                            std::chrono::milliseconds(500)
                        );

        }
    }
    return 0;
}
