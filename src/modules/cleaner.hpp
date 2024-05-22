#ifndef RENDERFORM_CLEANER_H
#define RENDERFORM_CLEANER_H

#include <iostream>
#include <opencv2/opencv.hpp>

namespace Renderform {
class Cleaner {
public:
  Cleaner(cv::Mat *image);
  ~Cleaner();

  void process();

  const cv::Mat getProcessedImage();

private:
  cv::Mat *source_image;
  cv::Mat processed_image;
};
} // namespace Renderform

#endif // RENDERFORM_CLEANER_H
