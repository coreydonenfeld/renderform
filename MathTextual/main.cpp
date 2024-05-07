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
                cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

  // White is component, black is background
  //
  // Need to determine if the text is white or black
  // One approach is to make the user specify the color of the text (kind of a
  // limitation and lame solution) Another approach is to use a color histogram
  // to determine the color of the text (more complex) Since

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
  tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

  // whitelist digits and operators
  // tess.SetVariable("tessedit_char_whitelist", "0123456789+-*/=():[]{}•x");
  tess.SetPageSegMode(tesseract::PSM_SINGLE_WORD);
  // tess.SetVariable("classify_bln_numeric_mode", "1");
  // tess.SetVariable("classify_enable_learning", "1");
  // tess.SetVariable("classify_enable_adaptive_matcher", "1");
  // tess.SetVariable("tessedit_char_blacklist", "`~@");
  tess.SetVariable("user_defined_dpi", "300");

  std::string recognizedText;

  for (int i = 1; i < nLabels; i++) {
    cv::Mat mask = labels == i;
    cv::Mat component;
    img.copyTo(component, mask);

    // Get bounding box
    cv::Rect boundingBox = cv::boundingRect(mask);

    // Get the text
    tess.SetImage(component.data, component.cols, component.rows, 1,
                  component.cols);
    tess.Recognize(0);
    recognizedText += tess.GetUTF8Text();

    // Draw bounding box
    cv::rectangle(img, boundingBox, cv::Scalar(0, 0, 255), 2, 4);
  }

  std::cout << "Recognized text: " << recognizedText << std::endl;

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
