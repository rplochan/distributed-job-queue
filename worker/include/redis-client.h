#pragma once 
#include <string>
 
class RedisClient {

private:
    void* context;

public:
    RedisClient();
    ~RedisClient();

    std::string getJob();
};