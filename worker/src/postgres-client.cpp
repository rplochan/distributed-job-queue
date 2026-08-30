#include "postgres-client.h"

#include <iostream>
#include <stdexcept>

PostgresClient::PostgresClient()
    : conn(
        "host=localhost "
        "port=5432 "
        "dbname=job_queue "
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
        "SET status = $1"
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
        "SET result = $1"
        "WHERE id = $2",
        result,
        jobId
    );
    transaction.commit();
}