#include "job-parser.h"

#include <sstream>
#include <stdexcept>

Job JobParser::parse(const std::string& data) {

    std::stringstream ss(data);

    std::string id;
    std::string type;
    std::string a;
    std::string b;
    std::string duration;

    std::getline(ss, id, '|');
    std::getline(ss, type, '|');
    std::getline(ss, a, '|');
    std::getline(ss, b, '|');
    std::getline(ss, duration, '|');

    if (id.empty() || type.empty()) {
        throw std::runtime_error("Invalid job format");
    }

    Job job;

    job.id = std::stoi(id);
    job.type = type;

    if (!a.empty()) {
        job.a = std::stoi(a);
    }

    if (!b.empty()) {
        job.b = std::stoi(b);
    }

    if (!duration.empty()) {
        job.duration = std::stoi(duration);
    }

    return job;
}