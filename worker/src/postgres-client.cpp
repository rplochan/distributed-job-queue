#include "../include/postgres-client.h"

#include <iostream>
#include <stdexcept>

PostgresClient::PostgresClient()
    : conn(
        "host=localhost "
        "port=5432 "
        "dbname=job-queue "
        "user=postgres "
        "password=chatapp123"
      )
{
    if (!conn.is_open()) {
        throw std::runtime_error(
            "Failed to connect to PostgreSQL"
        );
    }

    std::cout << "PostgreSQL connected!\n";
}

void PostgresClient::updateStatus(int jobId, const std::string& status){
    pqxx::work transaction(conn);

    transaction.exec_params(
        "UPDATE jobs"
        "SET status = $1, updated_at = CURRENT_TIMESTAMP"
        "WHERE id = $2",
        status,
        jobId
    );

    transaction.commit();
}

void PostgresClient::updateResult(int jobId, int result){
    pqxx::work transaction(conn);

    transaction.exec_params(
        "UPDATE jobs"
        "SET result = $1, updated_at = CURRENT_TIMESTAMP"
        "WHERE id = $2",
        result,
        jobId
    );
    transaction.commit();
}

void PostgresClient::updateTextResult(int jobId, const std::string& result){
    pqxx::work transaction(conn);

    transaction.exec_params(
        "UPDATE jobs"
        "SET result_text = $1, updated_at = CURRENT_TIMESTAMP"
        "WHERE id = $2",
        result,
        jobId
    );

    transaction.commit();
}

void PostgresClient::updateError(
    int jobId,
    const std::string& error)
{
    pqxx::work transaction(conn);

    transaction.exec_params(
        "UPDATE jobs "
        "SET error = $1, status = 'failed', "
        "updated_at = CURRENT_TIMESTAMP "
        "WHERE id = $2",
        error,
        jobId
    );

    transaction.commit();
}
