#pragma once
#include <hiredis/hiredis.h>
#include <stdexcept>
#include "../include/redis-client.h"
#include <cstdlib>

/*
C++ Worker
    │
    │ TCP connection
    ▼
127.0.0.1:6379
    │
    ▼
Valkey
*/

RedisClient::RedisClient() {
    const char* host = std::getenv("REDIS_HOST");
    const char* portEnv = std::getenv("REDIS_PORT");

    std::string redisHost = host ? host : "127.0.0.1";
    int redisPort = portEnv ? std::stoi(portEnv) : 6379;

    redisContext* redis = redisConnect(
        redisHost.c_str(),
        redisPort
    );

    if (redis == nullptr || redis->err) {
        throw std::runtime_error(
            "Failed to connect to Redis/Valkey"
        );
    }

    context = redis;
}

RedisClient::~RedisClient(){
    if(context){
        redisFree (
            static_cast<redisContext*>(context)
        );
    }
}

std::string RedisClient::getJob(){
    redisContext* redis = static_cast<redisContext*>(context);

    redisReply* reply = static_cast<redisReply*>(
        redisCommand(redis, "RPOP jobs")
    );

    if(reply == nullptr){
        throw std::runtime_error(
            "Failed to get job from Redis"
        );
    }

     std::string job;

    if (reply->type == REDIS_REPLY_STRING) {
        job = reply->str;
    }

    freeReplyObject(reply);

    return job;
}