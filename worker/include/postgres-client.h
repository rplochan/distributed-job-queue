#pragma once
#include <string>
#include <pqxx/pqxx>

class PostgresClient {
    private:
    pqxx::connection conn;

    public:
    PostgresClient();
    void updateStatus(int jobId, const std::string& status);
    void completeJob(int jobId, int result);
    void completeTextJob(int jobId, const std::string& result);
    void failJob(int jobId, const std::string& error);
};
