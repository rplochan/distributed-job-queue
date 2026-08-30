#include <chrono>
#include <iostream>
#include <thread>

#include "job-parser.h"
#include "processor.h"
#include "redis-client.h"
#include "postgres-client.h"

int main() {

    RedisClient redis;
    JobParser parser;
    JobProcessor processor;
    PostgresClient postgres;

    std::cout << "Worker started...\n";

    while (true) {

        try {

            // 1. Get a job from Redis
            std::string data = redis.getJob();

            // 2. No job available
            if (data.empty()) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(500)
                );

                continue;
            }

            std::cout << "Received job: "
                      << data << '\n';

            // 3. Convert string → Job
            Job job = parser.parse(data);

            // update the status in the postgresql
            postgres.updateStatus(job.id, "processing");

            // 4. Execute job
            int result = processor.process(job);

            // update the status in the postgresql
            postgres.updateResult(job.id, result);

            // mark completed in the postgresql
            postgres.updateStatus(
                job.id,
                "completed"
            );

            // 5. Display result
            std::cout << "Job "
                      << job.id
                      << " completed. Result: "
                      << result
                      << '\n';
        }

        catch (const std::exception& e) {

            std::cerr << "Error: "
                      << e.what()
                      << '\n';
        }
    }

    return 0;
}