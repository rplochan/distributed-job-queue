#pragma once 
#include <string>

class PDFGenerator {
public:
    std::string generate(
        int jobId,
        const std::string& title,
        const std::string& content
    );
};