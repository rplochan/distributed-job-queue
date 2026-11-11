#pragma once
#include <string>

class ImageResizer {
public:
    std::string resize(int jobId, const std::string& inputPath, int width, int height);
};