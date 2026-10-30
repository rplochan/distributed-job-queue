#pragma once


#include "../include/job.h"
#include <string>
class JobProcessor {
public:
    int process(const Job& job);
    std::string processHash(const Job& job);
};
