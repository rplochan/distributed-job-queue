#pragma once

#include "job.h"
#include <string>

class JobParser {
public:
    Job parse(const std::string& data);
};