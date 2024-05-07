#include "tesseract/publictypes.h"
#include <iostream>
#include <leptonica/allheaders.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <image>" << std::endl;
    exit(EXIT_FAILURE);
  }

  cv::Mat img = cv::imread(argv[1], cv::IMREAD_COLOR);
  if (img.empty()) {
    std::cerr << "Error: <input_image> not found." << std::endl;
    exit(EXIT_FAILURE);
  }

  cv::Mat img_gray;
  cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

  // Thresholding to separate background and foreground
  double threshold = 0; // Example threshold value

  cv::Mat binary;
  cv::threshold(img_gray, binary, threshold, 255,
                cv::THRESH_BINARY | cv::THRESH_OTSU);

  // // Count black and white pixels in the binary image
  // int blackPixels = cv::countNonZero(binary);
  // int whitePixels = binary.rows * binary.cols - blackPixels;

  // // Determine background color based on pixel counts
  // std::string backgroundColor = (blackPixels > whitePixels) ? "White" :
  // "Black"; std::cout << "Background color: " << backgroundColor << std::endl;

  // Get connected components
  cv::Mat labels, stats, centroids;
  int nLabels =
      cv::connectedComponentsWithStats(binary, labels, stats, centroids);

  std::cout << "Input image size: " << binary.size() << std::endl;
  std::cout << "Number of labels: " << nLabels << std::endl;

  // Debug print stats and centroids
  std::cout << "Stats: " << stats << std::endl;
  std::cout << "Centroids: " << centroids << std::endl;

  // use tesseract to get OCR for each connected component
  tesseract::TessBaseAPI tess;
  // tess.Init(NULL, "eng");
  tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

  // remove unconnected components in region of interest @todo

  // whitelist digits and operators
  // tess.SetVariable("tessedit_char_whitelist", "0123456789+-*/=()");
  // tess.SetVariable("tessedit_char_whitelist", "0123456789");
  // tess.SetPageSegMode(tesseract::PSM_SPARSE_TEXT_OSD);
  // tess.SetPageSegMode(tesseract::PSM_AUTO);
  tess.SetPageSegMode(tesseract::PSM_SINGLE_WORD);
  tess.SetVariable("classify_bln_numeric_mode", "1");
  // tess.SetVariable("classify_enable_learning", "0");
  // tess.SetVariable("classify_enable_adaptive_matcher", "0");
  // tess.SetVariable("tessedit_char_whitelist", "0123456789:+-*/=()");
  // tess.SetVariable("tessedit_char_blacklist",
  // "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  // tess.SetVariable("user_defined_dpi", "300");
  //

  std::string recognizedText;

  // Invert the image
  // if (backgroundColor == "Black")
  //   cv::bitwise_not(binary, binary);

  // First pass: Draw red bounding boxes around individual connected components
  for (int i = 0; i < nLabels; i++) {
    // break;
    cv::Rect bbox(stats.at<int>(i, cv::CC_STAT_LEFT),
                  stats.at<int>(i, cv::CC_STAT_TOP),
                  stats.at<int>(i, cv::CC_STAT_WIDTH),
                  stats.at<int>(i, cv::CC_STAT_HEIGHT));
    cv::rectangle(img, bbox, cv::Scalar(0, 0, 255), 1,
                  cv::LINE_4); // Draw red bounding box

    cv::Mat roi = binary(bbox).clone();

    // if (backgroundColor == "Black")
    //   cv::bitwise_not(roi, roi);

    tess.SetImage(roi.data, roi.cols, roi.rows, 1, roi.step);
    tess.Recognize(0);
    recognizedText += tess.GetUTF8Text();
    std::cout << i << ": " << tess.GetUTF8Text() << std::endl;

    // draw the OCR text on the image in bottom left corner of bounding box
    // cv::putText(img, tess.GetUTF8Text(),
    //             cv::Point(bbox.x + 5, bbox.y + bbox.height - 5),
    //             cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 1);

    cv::imshow("roi " + std::to_string(i), roi);

    // // draw the index of the connected component in the top left corner of
    // the
    // bounding box
    // cv::putText(img, std::to_string(i), cv::Point(bbox.x, bbox.y + 12),
    //             cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 1);
  }

  // tess.SetImage(binary.data, binary.cols, binary.rows, 1, binary.step);
  // tess.Recognize(0);
  // recognizedText += tess.GetUTF8Text();

  std::cout << "Recognized text: " << recognizedText << std::endl;

  // cv::imwrite("test-output.png", img);

  // // Display the result
  cv::imshow("Bounding Boxes", img);
  cv::imshow("Pre-processed image", binary);
  cv::waitKey(0);

  return 0;
}

/*

current idea, not sure if or how this fits into the opencv/tesseract schemes:
- get image and threshold and do image pre-processing
- detect connected-components / symbols
- segment connected components into lines
- run each through line and its characters through tesseract to get OCR for
characters (not sure how this works for square root, etc.?)
- maybe for matrices or square root - would need something in opencv / another
AI library to detect those (could be v2)
- use position and scale/size to determine what the equation is
  - bases/subscript is small and on lower half
  - exponents/superscript is small and on upper half

*/
