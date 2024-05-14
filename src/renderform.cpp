#include "renderform.hpp"

Renderform::Formula Renderform::parse(const std::string &image_path) {
  Renderform::Formula parsed_formula;

  cv::Mat image_src = cv::imread(image_path, cv::IMREAD_COLOR);
  if (image_src.empty()) {
    std::cerr << "Error: <image> not found or could not be read." << std::endl;
    return parsed_formula;
  }

  cv::imshow("Original Input Image", image_src);
  cv::waitKey(0);

  return parsed_formula;
}
