
#include "../include/processor.h"
#include "../include/hash-utils.h"
#include <chrono>
#include <limits>
#include <stdexcept>
#include <thread>

int JobProcessor::process(const Job& job)
{
    if (job.type == "add") {
        long long result = static_cast<long long>(job.a) + job.b;

        if (result < std::numeric_limits<int>::min() ||
            result > std::numeric_limits<int>::max()) {
            throw std::overflow_error("addition overflow");
        }

        return static_cast<int>(result);
    }

    if (job.type == "subtract") {
        long long result = static_cast<long long>(job.a) - job.b;

        if (result < std::numeric_limits<int>::min() ||
            result > std::numeric_limits<int>::max()) {
            throw std::overflow_error("subtraction overflow");
        }

        return static_cast<int>(result);
    }

    if (job.type == "mult" || job.type == "multiply") {
        long long result = static_cast<long long>(job.a) * job.b;

        if (result < std::numeric_limits<int>::min() ||
            result > std::numeric_limits<int>::max()) {
            throw std::overflow_error("multiplication overflow");
        }

        return static_cast<int>(result);
    }

    if (job.type == "div") {
        if (job.b == 0) {
            throw std::runtime_error("division by zero");
        }

        if (job.a == std::numeric_limits<int>::min() && job.b == -1) {
            throw std::overflow_error("division overflow");
        }

        return job.a / job.b;
    }

    if (job.type == "power") {
        if (job.b < 0) {
            throw std::runtime_error("negative exponent unsupported");
        }

        long long result = 1;

        for (int i = 0; i < job.b; ++i) {
            result *= job.a;

            if (result < std::numeric_limits<int>::min() ||
                result > std::numeric_limits<int>::max()) {
                throw std::overflow_error("power overflow");
            }
        }

        return static_cast<int>(result);
    }

    if (job.type == "factorial") {
        if (job.a < 0) {
            throw std::runtime_error("factorial requires non-negative input");
        }

        int result = 1;

        for (int i = 2; i <= job.a; ++i) {
            if (result > std::numeric_limits<int>::max() / i) {
                throw std::overflow_error("factorial overflow");
            }

            result *= i;
        }

        return result;
    }

    if (job.type == "fibonacci") {
        if (job.a < 0) {
            throw std::runtime_error("fibonacci requires non-negative input");
        }

        int previous = 0;
        int current = 1;

        for (int i = 0; i < job.a; ++i) {
            if (previous > std::numeric_limits<int>::max() - current) {
                throw std::overflow_error("fibonacci overflow");
            }

            int next = previous + current;
            previous = current;
            current = next;
        }

        return previous;
    }

    if (job.type == "prime_check") {
        if (job.a < 2) {
            return 0;
        }

        for (int i = 2; i <= job.a / i; ++i) {
            if (job.a % i == 0) {
                return 0;
            }
        }

        return 1;
    }

    if (job.type == "sleep") {
        if (job.duration < 0) {
            throw std::runtime_error("sleep duration cannot be negative");
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(job.duration)
        );

        return 0;
    }

    throw std::runtime_error("unknown job type: " + job.type);
}

std::string JobProcessor::processHash(const Job &job){
    if(job.type != "hash"){
        throw std::runtime_error(
            "processHash called for non-hash job"
        );
    }
    return sha256(job.input);
}
