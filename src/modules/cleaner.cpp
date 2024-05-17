/*

Need to take an input image and clean it up before getting each character.

At an abstract level it is a matter of separating the foreground from the
background.

The foreground is the text and the background is everything else.

Some assumptions:
- The background is mostly uniform.
- The text is mostly uniform (each character is mostly the same color)
- The background will have lines or a grid pattern (possibly NO pattern is the
easiest case)
- The background will have noise (dust, scratches, etc)
- There may be scratched out text
- The background may include patterns or sketches that are not crucial to the
image
*/

#include "cleaner.hpp"
#include <opencv2/imgproc.hpp>

Renderform::Cleaner::Cleaner(cv::Mat *image) : source_image(image) {
  cv::imshow("Original Image cleaning", *image);
  cv::waitKey(0);
  this->processed_image = cv::Mat{};
}

Renderform::Cleaner::~Cleaner() {}

void Renderform::Cleaner::process() {
  /* Step 1: Convert to grayscale. */
  cv::cvtColor(*(this->source_image), this->processed_image,
               cv::COLOR_BGR2GRAY);

  cv::imshow("Grayscale Image", this->processed_image);
  cv::waitKey(0);

  // Subtract blurred from grayscale
  // cv::Mat blurred;
  // cv::GaussianBlur(this->processed_image, blurred, cv::Size(7, 7), 4);
  // cv::subtract(this->processed_image, blurred, this->processed_image);

  // cv::subtract(blurred, this->processed_image, this->processed_image);

  // cv::imshow("Blurred Image", blurred);
  // cv::waitKey(0);

  // /* Step 2: Detect and filter out noise from the background. */
  cv::Mat structuring_element =
      cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(13, 5));
  // // ^^^ Mostly works but I need to understand what is happening
  // cv::Mat background;
  // cv::morphologyEx(this->processed_image, background, cv::MORPH_CLOSE,
  //                  structuring_element);

  // cv::threshold(background, background, 200, 255,
  //               cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

  // cv::imshow("Background", background);
  // cv::waitKey(0);

  // // cv::divide(this->processed_image, background, this->processed_image,
  // // 255.0);
  // cv::subtract(background, this->processed_image, this->processed_image);
  // // cv::subtract(this->processed_image, background, this->processed_image);

  // cv::erode(this->processed_image, this->processed_image,
  // structuring_element,
  //           cv::Point(-1, -1), 2);

  // cv::imshow("Dilated Image", this->processed_image);
  // cv::waitKey(0);

  /* Step 3: Threshold the image. */
  cv::threshold(this->processed_image, this->processed_image, 90, 255,
                cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

  // cv::adaptiveThreshold(this->processed_image, this->processed_image, 255,
  //                       cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 39,
  //                       13);
}

const cv::Mat Renderform::Cleaner::getProcessedImage() {
  return this->processed_image;
}
