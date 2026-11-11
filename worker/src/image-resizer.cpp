#include "../include/image-resizer.h"

#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <string>

std::string ImageResizer::resize(
    int jobId,
    const std::string& inputPath,
    int width,
    int height
) {
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid image dimensions");
    }

    cv::Mat image = cv::imread(inputPath);

    if (image.empty()) {
        throw std::runtime_error("Could not load input image");
    }

    cv::Mat resized;

    cv::resize(
        image,
        resized,
        cv::Size(width, height)
    );

    std::string outputPath =
        "output/resized_" + std::to_string(jobId) + ".png";

    if (!cv::imwrite(outputPath, resized)) {
        throw std::runtime_error("Failed to save resized image");
    }

    return outputPath;
}