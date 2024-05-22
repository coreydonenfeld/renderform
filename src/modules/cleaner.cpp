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

Renderform::Cleaner::Cleaner(cv::Mat *image) : source_image(image) {
  this->processed_image = cv::Mat{};
}

Renderform::Cleaner::~Cleaner() {}

void Renderform::Cleaner::process() {
  /* Step 1: Convert to grayscale. */
  cv::cvtColor(*(this->source_image), this->processed_image,
               cv::COLOR_BGR2GRAY);

  cv::Mat paper;
  cv::Mat background_paper;

  cv::GaussianBlur(this->processed_image, paper, cv::Size(7, 7), 2, 2);

  // Get brightness/intensity histogram of grayscale image.
  cv::Mat hist;
  int histSize = 256;
  float range[] = {0, 256};
  const float *histRange = {range};
  cv::calcHist(&paper, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true,
               false);

  // get the brightest color
  int brightest = 0;
  for (int i = 255; i >= 0; i--) {
    if (hist.at<float>(i) > 0) {
      brightest = i;
      break;
    }
  }

  // get the darkest color
  int darkest = 0;
  for (int i = 0; i < 256; i++) {
    if (hist.at<float>(i) > 0) {
      darkest = i;
      break;
    }
  }

  // get the average color
  int average = 0;
  int total = 0;
  for (int i = 0; i < 256; i++) {
    total += hist.at<float>(i) * i;
  }
  average = total / paper.total();

  // get most used color
  int most_used = 0;
  int most_used_count = 0;
  for (int i = 0; i < 256; i++) {
    if (hist.at<float>(i) > most_used_count) {
      most_used = i;
      most_used_count = hist.at<float>(i);
    }
  }

  // get most used color < 128
  int most_used_dark = 0;
  int most_used_dark_count = 0;
  for (int i = 0; i < 128; i++) {
    if (hist.at<float>(i) > most_used_dark_count) {
      most_used_dark = i;
      most_used_dark_count = hist.at<float>(i);
    }
  }

  // get most used color > 128
  int most_used_light = 0;
  int most_used_light_count = 0;
  for (int i = 128; i < 256; i++) {
    if (hist.at<float>(i) > most_used_light_count) {
      most_used_light = i;
      most_used_light_count = hist.at<float>(i);
    }
  }

  // Define the threshold value
  int threshold_value = 110;
  cv::Mat binary_mask;

  // Apply the threshold
  cv::threshold(this->processed_image, binary_mask, threshold_value, 255,
                cv::THRESH_BINARY);

  // Use the binary mask to extract the bright pixels
  cv::Mat bright_pixels;
  this->processed_image.copyTo(bright_pixels, binary_mask);

  cv::dilate(bright_pixels, bright_pixels, cv::Mat(), cv::Point(-1, -1), 11);
  cv::GaussianBlur(bright_pixels, bright_pixels, cv::Size(7, 7), 3, 3);
  cv::threshold(bright_pixels, bright_pixels, 120, 255, cv::THRESH_BINARY);

  // Get biggest contour
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(bright_pixels, contours, hierarchy, cv::RETR_TREE,
                   cv::CHAIN_APPROX_SIMPLE);

  int largest_area = 0;
  int largest_contour_index = 0;
  for (int i = 0; i < contours.size(); i++) {
    double a = cv::contourArea(contours[i], false);
    if (a > largest_area) {
      largest_area = a;
      largest_contour_index = i;
    }
  }

  cv::Mat drawing = cv::Mat::zeros(bright_pixels.size(), CV_8UC1);
  cv::drawContours(drawing, contours, largest_contour_index, cv::Scalar(255),
                   cv::FILLED, 8, hierarchy);

  // use drawing as mask for foreground_paper
  cv::Mat foreground_paper;
  foreground_paper = cv::Mat::zeros(this->processed_image.size(), CV_8UC1);
  cv::bitwise_and(this->processed_image, drawing, foreground_paper);

  /* Processed image only contains the portions from the inside of the paper
   * (attempts to remove any non-paper background). */
  this->processed_image = foreground_paper;

  /* Step 2: Detect and filter out noise from the background. */
  /* Get horizontal lines */
  cv::Mat structuring_element_horizontal =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(71, 1));
  cv::Mat structuring_element_vertical =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 37));
  // cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 121));

  // Get extremely strong horizontal lines (edges)
  cv::Mat horizontal_lines;
  cv::Mat vertical_lines;
  cv::morphologyEx(this->processed_image, horizontal_lines, cv::MORPH_CLOSE,
                   structuring_element_horizontal, cv::Point(-1, -1), 2);
  cv::morphologyEx(this->processed_image, vertical_lines, cv::MORPH_CLOSE,
                   structuring_element_vertical, cv::Point(-1, -1), 2);

  cv::Mat foreground;

  // Foreground is what is in the image that is not in the horizontal lines or
  // vertical lines

  cv::Mat background;

  // get both horizontal and vertical lines
  cv::scaleAdd(horizontal_lines, 0.5, vertical_lines, background);

  cv::subtract(horizontal_lines, this->processed_image, foreground);

  /* Step 3: Threshold the image. */
  cv::threshold(foreground, this->processed_image, 60, 255,
                cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

  cv::subtract(background, this->processed_image, this->processed_image);

  cv::Mat border_strengthen =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  cv::dilate(this->processed_image, this->processed_image, border_strengthen,
             cv::Point(-1, -1), 1);

  // @debug [show processed image]
  cv::imshow("Foreground", this->processed_image);
  cv::waitKey(1);
}

const cv::Mat Renderform::Cleaner::getProcessedImage() {
  return this->processed_image;
}
