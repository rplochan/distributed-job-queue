#include "processor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

int JobProcessor::process(const Job& job){

    if(job.type == "add"){
        return job.a + job.b;
    }

     if (job.type == "multiply") {
        return job.a * job.b;
    }


    if (job.type == "sleep") {
        std::this_thread::sleep_for(
            std::chrono::seconds(job.duration)
        );
        return 0;
    }

        
    throw std::runtime_error("unknown job type");

     
}
