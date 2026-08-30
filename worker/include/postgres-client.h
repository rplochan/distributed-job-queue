#pragma once
#include <string>
#include <pqxx/pqxx>

class PostgresClient {
    private:
    pqxx::connection conn;

    public:
    PostgresClient();
    void updateStatus(int jobId, const std::string& status);
    void updateResult(int jobId, int result);
};