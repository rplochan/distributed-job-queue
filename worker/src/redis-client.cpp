#pragma once
#include <hiredis/hiredis.h>
#include <stdexcept>
#include "redis-client.h"

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

RedisClient::RedisClient(){
    redisContext* redis = redisConnect("127.0.0.1", 6379);

    if(redis == nullptr || redis->err){
        throw std::runtime_error(
            "failed to connect to redis"
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