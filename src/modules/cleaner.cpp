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
#include <opencv2/photo.hpp>

Renderform::Cleaner::Cleaner(cv::Mat *image) : source_image(image) {
  // cv::imshow("Original Image cleaning", *image);
  // cv::waitKey(0);
  this->processed_image = cv::Mat{};
}

Renderform::Cleaner::~Cleaner() {}

void Renderform::Cleaner::process() {
  /* Step 1: Convert to grayscale. */
  cv::cvtColor(*(this->source_image), this->processed_image,
               cv::COLOR_BGR2GRAY);

  // cv::imshow("Grayscale Image", this->processed_image);
  // cv::waitKey(0);

  // Need to get ONLY characters on page and remove everything else

  // Canny edge detection
  // cv::Mat edges;
  // // cv::Canny(this->processed_image, edges, 107, 80);

  // cv::Mat sobelx, sobely, sobelxy;
  // cv::Sobel(this->processed_image, sobelx, CV_64F, 1, 0, 3);
  // cv::Sobel(this->processed_image, sobely, CV_64F, 0, 1, 3);
  // cv::Sobel(this->processed_image, sobelxy, CV_64F, 1, 1, 3);

  // // cv::imshow("Sobelx", sobelx);
  // // cv::waitKey(0);
  // // cv::imshow("Sobely", sobely);
  // // cv::waitKey(0);
  // // cv::imshow("Sobel", sobelxy);
  // // cv::waitKey(0);

  // cv::magnitude(sobelx, sobely, sobelxy);
  // cv::convertScaleAbs(sobelxy, edges);

  // // enhance edges
  // // cv::dilate(edges, edges, cv::Mat{}, cv::Point(-1, -1), 2);
  // // cv::GaussianBlur(edges, edges, cv::Size(9, 9), 12, 4);
  // // cv::threshold(edges, edges, 0, 255, cv::THRESH_BINARY |
  // cv::THRESH_OTSU);

  // cv::imshow("Edges", edges);
  // cv::waitKey(0);

  // cv::Mat foreground;

  // // cv::multiply(this->processed_image, edges, foreground);
  // cv::threshold(edges, foreground, 40, 255, cv::THRESH_BINARY);
  // cv::imshow("Foreground", foreground);
  // cv::waitKey(0);

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
