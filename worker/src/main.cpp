#include <iostream>

#include "job.h"
#include "processor.h"

int main() {

    JobProcessor processor;

    Job job1{1, "add", 10, 20, 0};

    int result = processor.process(job1);

    std::cout << "Job " << job1.id << " result: " << result << '\n';

    return 0;
}