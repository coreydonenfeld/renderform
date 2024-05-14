#ifndef RENDERFORM_CLEANER_H
#define RENDERFORM_CLEANER_H

#include <opencv2/opencv.hpp>

namespace Renderform {
class Cleaner {
public:
  Cleaner(cv::Mat &image);
  ~Cleaner();
};
private:
  cv::Mat *image;
};
} // namespace Renderform

#endif // RENDERFORM_CLEANER_H
