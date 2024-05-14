#include "tesseract/publictypes.h"
#include <iostream>
#include <leptonica/allheaders.h>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/photo.hpp>
#include <tesseract/baseapi.h>

#define TESSERACT_OUTPUT_FILE "output.txt"

int main(int argc, char **argv) {
  if (2 > argc) {
    std::cerr << "Usage: " << argv[0] << " <image>" << std::endl;
    exit(EXIT_FAILURE);
  }

  cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
  if (image.empty()) {
    std::cerr << "Error: <input_image> not found or could not be read."
              << std::endl;
    exit(EXIT_FAILURE);
  }
  cv::Mat image_copy = image.clone();

  // Convert the input image to grayscale
  cv::Mat grayscale;
  cv::cvtColor(image, grayscale, cv::COLOR_BGR2GRAY);

  // Get average color of the image
  cv::Scalar avg_color = cv::mean(grayscale);
  std::cout << "Average color: " << avg_color << std::endl;

  cv::imshow("Grayscale", grayscale);
  cv::waitKey(0);

  // threshold the image based on the average color
  cv::threshold(grayscale, grayscale, avg_color[0], 255,
                cv::THRESH_BINARY | cv::THRESH_OTSU);

  // Get the connected components of the image
  cv::Mat grayscale_labels, grayscale_stats, grayscale_centroids;
  int grayscale_components = cv::connectedComponentsWithStats(
      grayscale, grayscale_labels, grayscale_stats, grayscale_centroids);

  for (int i = 1; i < grayscale_components; i++) {
    std::cout << "Component " << i << " stats: " << grayscale_stats.row(i)
              << std::endl;
  }

  cv::imshow("Thresholded", grayscale);
  cv::waitKey(0);

  // cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  // cv::morphologyEx(grayscale, grayscale, cv::MORPH_CLOSE, kernel);

  // Grayscale contours
  cv::Mat shapes_contours = cv::Mat::zeros(grayscale.size(), CV_8UC1);
  std::vector<std::vector<cv::Point>> grayscale_contours;
  cv::findContours(grayscale, grayscale_contours, cv::RETR_TREE,
                   cv::CHAIN_APPROX_SIMPLE);
  cv::drawContours(shapes_contours, grayscale_contours, -1, cv::Scalar(255), 2);

  cv::imshow("Grayscale Contours", shapes_contours);
  cv::waitKey(0);

  // Separate the foreground from the background
  cv::Mat foreground; // Pen/pencil characters
  cv::Mat background; // Lines, paper, edges, noise, etc.

  // We need to separate the foreground from the background

  // Before thresholding, remove straight horizontal lines >70% of the width of
  // the image
  // Step 1: Apply a horizontal kernel to the image
  // Step 2: Apply a morphological operation to remove the horizontal lines

  cv::Mat horizontal_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8, 1));

  cv::Mat vertical_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 6));

  cv::dilate(grayscale, background, horizontal_kernel, cv::Point(-1, -1), 1);
  cv::dilate(background, background, horizontal_kernel, cv::Point(-1, -1), 2);

  // cv::morphologyEx(background, background, cv::MORPH_DILATE,
  // horizontal_kernel); cv::morphologyEx(background, background,
  // cv::MORPH_CLOSE, horizontal_kernel);

  // cv::dilate(background, background, horizontal_kernel, cv::Point(-1, -1),
  // 1); cv::erode(background, background, horizontal_kernel, cv::Point(-1, -1),
  // 1);

  // subtract background from the original image
  cv::subtract(background, grayscale, foreground);

  cv::threshold(foreground, foreground, 40, 255, cv::THRESH_BINARY_INV);
  cv::GaussianBlur(foreground, foreground, cv::Size(5, 5), 0, 13);
  cv::threshold(foreground, foreground, 0, 255, cv::THRESH_OTSU);

  cv::Mat image_edges;
  cv::Canny(foreground, image_edges, 50, 150, 3);

  cv::Mat image_window_display;

  // cv::namedWindow("Input Image Grayscale", cv::WINDOW_NORMAL);
  // cv::imshow("Input Image Grayscale", grayscale);
  // cv::imshow("Horizontal Lines", image_edges);
  // cv::imshow("Foreground", foreground);
  // cv::imshow("Background", background);
  // cv::resizeWindow("Input Image Grayscale", 800, 800);
  // cv::waitKey(0);

  // Check for VERY straight horizontal lines (i.e. the paper edge or lines on
  // paper as opposed to actual text)

  // check for continuous straight lines that span approx the width of the image
  // (or > 75% of the width of the image) if found, then it's likely a paper
  // edge or a line on the paper if found, then remove it from the image

  // cv::waitKey(0);

  // apply gaussian blur to remove noise
  // cv::GaussianBlur(grayscale, grayscale, cv::Size(9, 9), 1, 3);

  // cv::imshow("Gaussian Blur", grayscale);
  // cv::waitKey(0);

  // Pen/pencil should be the darkest object in the image
  // Let's find the darkest pixel in the image and then see what that looks like
  // to determine the threshold
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(grayscale, &minVal, &maxVal);

  cv::Mat thresholded;
  // cv::threshold(grayscale, thresholded, minVal + 60, 255,
  //               cv::THRESH_BINARY | cv::THRESH_OTSU);
  // cv::adaptiveThreshold(grayscale, thresholded, 255,
  //                       cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY,
  //                       37, (maxVal - minVal) / 4);

  // cv::imshow("Thresholded", thresholded);
  // cv::waitKey(0);

  // std::cout << "Min Value: " << minVal << std::endl;
  // std::cout << "Max Value: " << maxVal << std::endl;

  // Perform dilation and erosion to enhance edges
  cv::Mat dilated_edges, eroded_edges;
  // cv::dilate(thresholded, dilated_edges, cv::Mat(), cv::Point(-1, -1), 1);
  // cv::erode(grayscale, eroded_edges, cv::Mat(), cv::Point(-1, -1), 1);

  // cv::imshow("Dilated Edges", eroded_edges);
  // cv::waitKey(0);

  cv::threshold(grayscale, thresholded, minVal, 255,
                cv::THRESH_BINARY | cv::THRESH_OTSU);
  // cv::adaptiveThreshold(eroded_edges, thresholded, 255,
  //                       cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY,
  //                       37, (maxVal - minVal) / 4);
  cv::imshow("Thresholded", thresholded);
  cv::waitKey(0);

  // Apply Canny edge detection
  cv::Mat edges;
  cv::Canny(thresholded, edges, 200, 250); // Adjust thresholds as needed

  cv::imshow("Edges", edges);
  cv::waitKey(0);

  // Apply connected component labeling
  cv::Mat labels, stats, centroids;
  int nLabels =
      cv::connectedComponentsWithStats(edges, labels, stats, centroids);

  // use tesseract to get OCR for each connected component
  tesseract::TessBaseAPI tess;
  tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
  tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  // tess.SetVariable("classify_bln_numeric_mode", "1");

  // whitelist digits and operators
  tess.SetVariable("tessedit_char_whitelist", "0123456789=+-*/xyzXYZ");
  // tess.SetVariable("tessedit_char_whitelist", "+-=-~—x*•");
  // tess.SetVariable("user_defined_dpi", "300");
  // tess.SetVariable("matcher_good_threshold", "0");
  // tess.SetVariable("classify_enable_learning", "1");
  // tess.SetVariable("classify_enable_adaptive_matcher", "1");
  // tess.SetVariable("tessedit_char_blacklist", "`~@");

  std::string recognizedText;

  int start_component_i = 1;
  int end_component_i = nLabels;
  int user_start_component_i = 0;
  int user_end_component_i = 0;

  if (argc == 4) {
    user_start_component_i = std::stoi(argv[2]);
    user_end_component_i = std::stoi(argv[3]);
    if (user_start_component_i < 0 || user_end_component_i < 0 ||
        user_start_component_i >= nLabels || user_end_component_i >= nLabels) {
      std::cout << "Warning: Invalid component index. Using default range."
                << std::endl;
    } else {
      start_component_i = user_start_component_i;
      end_component_i = user_end_component_i;
    }
  }

  // Process each connected component
  for (int i = start_component_i; i < end_component_i; i++) {
    std::cout << "╭ Determining <Object " << i << "> Character" << std::endl;

    const int PADDING = 9;

    cv::Rect bbox(stats.at<int>(i, cv::CC_STAT_LEFT),
                  stats.at<int>(i, cv::CC_STAT_TOP),
                  stats.at<int>(i, cv::CC_STAT_WIDTH),
                  stats.at<int>(i, cv::CC_STAT_HEIGHT));

    // draw box on original image for demo
    cv::rectangle(image, bbox, cv::Scalar(0, 255, 0), 2);

    // create new tmp image that is the bounding box of the connected component
    // + padding to draw the shape in the center
    cv::Mat character = cv::Mat::zeros(bbox.height + 2 * PADDING,
                                       bbox.width + 2 * PADDING, CV_8UC1);
    cv::Mat component_outline = (labels(bbox) == i);
    // add the connected component to the tmp image
    component_outline.copyTo(
        character(cv::Rect(PADDING, PADDING, bbox.width, bbox.height)));

    // Threshold and binarize the original image
    // (Currently assumes no overlap - this might need to be adjusted and fixed
    // to refind ONLY the connected component in the original image)
    cv::Mat character_thresholded;
    cv::threshold(character, character_thresholded, 0, 255,
                  cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    cv::erode(character_thresholded, character_thresholded, cv::Mat(),
              cv::Point(-1, -1), 1);

    // get contours of the connected component
    std::vector<std::vector<cv::Point>> character_contours;
    cv::findContours(character_thresholded, character_contours, cv::RETR_TREE,
                     cv::CHAIN_APPROX_SIMPLE);

    if (character_contours.size() == 0) {
      std::cout << "No contours found for component " << i << std::endl;
      continue;
    }

    std::cout << "Number of contours: " << character_contours.size()
              << std::endl;

    cv::Mat character_demo;
    // get contour of the shape not the bounding box
    // make thresholded image rgb to draw contours
    cv::cvtColor(character_thresholded, character_demo, cv::COLOR_GRAY2BGR);

    // cv::drawContours(character_demo, character_contours, 1,
    //                  cv::Scalar(0, 255, 0), -1);

    // cv::drawContours(character_demo, character_contours, 2,
    //                  cv::Scalar(0, 0, 255), 1);

    cv::drawContours(character_thresholded, character_contours, 1,
                     cv::Scalar(0), -1);

    for (int j = 2; j < character_contours.size(); j++) {
      cv::drawContours(character_thresholded, character_contours, j,
                       cv::Scalar(0), 1);
    }

    // De-pixelate the character
    // cv::GaussianBlur(character_thresholded, character_thresholded,
    //                  cv::Size(13, 13), 0, 0);
    // cv::erode(character_thresholded, character_thresholded, cv::Mat(),
    //           cv::Point(-1, -1), 1);

    // fill in outline to get a solid shape
    // Invert thresholded image
    // cv::bitwise_not(character_thresholded, character_thresholded);

    // Apply dilation and erosion to the thresholded image
    int dilation_size = 1;
    int dilation_type = cv::MORPH_ERODE;
    cv::Mat element = getStructuringElement(
        dilation_type, cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
        cv::Point(dilation_size, dilation_size));

    // cv::erode(character_thresholded, character_thresholded, element,
    //           cv::Point(-1, -1), 1);
    // cv::dilate(character_thresholded, character_thresholded, element,
    //            cv::Point(-1, -1), 1);

    cv::imshow("Character Bounding Box " + std::to_string(i),
               character_thresholded);
    cv::waitKey(0);

    tess.SetImage(character_thresholded.data, character_thresholded.cols,
                  character_thresholded.rows, 1, character_thresholded.cols);
    tess.SetSourceResolution(70);

    tess.Recognize(0);

    std::string recognizedChar;
    std::cout << "├┄┄ Tesseract OCR" << std::endl;
    if (tess.MeanTextConf() > 0) {
      recognizedChar = tess.GetUTF8Text();
      // remove newlines and other whitespace characters
      recognizedChar.erase(
          std::remove_if(recognizedChar.begin(), recognizedChar.end(),
                         [](unsigned char x) { return std::isspace(x); }),
          recognizedChar.end());
      std::cout << "│\t↳ Text: " << recognizedChar << std::endl;
      std::cout << "│\t☑ Confidence: " << tess.MeanTextConf() << std::endl;
    } else {
      std::cout << "│\t☒ No text detected" << std::endl;
    }

    recognizedText += recognizedChar;
    recognizedText += " ";

    // Assuming the connected component is a single character, we can use the
    // properties of the character and do some non-OCR based determination below
    // is AI generated and most definitely not accurate but is a good starting
    // point for example, if the height is greater than the width, it is likely
    // a 1, 4, 7, or 9 if the height is less than the width, it is likely a 0,
    // 2, 3, 5, 6, or 8 if the height is equal to the width, it is likely a 1,
    // 4, 7, or 9 if the height is less than 1.5 times the width, it is likely a
    // 1, 4, 7, or 9 if the height is greater than 1.5 times the width, it is
    // likely a 0, 2, 3, 5, 6, or 8 if the height is greater than 2 times the
    // width, it is likely a 0, 2, 3, 5, 6, or 8 if the height is greater than 3
    // times the width, it is likely a 0, 2, 3, 5, 6, or 8
    //
    // Back to human:
    // if it is a long stick with a small hat, it is likely a 1
    //

    // Calculate properties of the thresholded image
    // a. Aspect ratio
    // b. Area
    // c. Height
    // d. Width
    // e. Number of holes
    // f. Number of edges
    // g. Number of corners
    // h. Number of intersections
    // i. Number of loops
    // j. Number of endpoints
    // k. Circularity
    // l. straightness
    // m. thickness
    // n. slant
    //
    std::cout << "├┄┄ Object Properties" << std::endl;

    double width = bbox.width;
    double height = bbox.height;
    double aspectRatio = width / height;
    double area = cv::countNonZero(character_thresholded);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(character_thresholded, contours, cv::RETR_CCOMP,
                     cv::CHAIN_APPROX_SIMPLE);

    std::cout << "│\tHeight: " << height << std::endl;
    std::cout << "│\tWidth: " << width << std::endl;

    std::cout << "│\tNumber of raw contours: " << contours.size() << std::endl;

    // draw contours to visualize
    // make copy of input image, bound it with the bounding box and draw
    // contours
    cv::Mat demo_character_contours = character.clone();
    // change character to color for visualization
    cv::cvtColor(demo_character_contours, demo_character_contours,
                 cv::COLOR_GRAY2BGR);
    // add padding for demo purposes
    cv::rectangle(demo_character_contours, bbox, cv::Scalar(0, 255, 0), 2);
    cv::drawContours(demo_character_contours, contours, -1,
                     cv::Scalar(0, 0, 255), 2);

    std::vector<std::vector<cv::Point>> filtered_contours;
    int contour_i = 0;
    for (const auto &contour : contours) {
      contour_i++;

      std::cout << "│\t╰┄┄ Contour " << contour_i << " (size " << contour.size()
                << ")" << std::endl;

      // Compute contour area
      double area = cv::contourArea(contour);

      // if 4 points and the x is 0 or width and y is 0 or height, it is the
      // border and should be ignored
      if (contour.size() == 4) {
        bool isBorder = false;
        std::cout << "│\t\tContour is a rectangle (FIRST CHECK)" << std::endl;
        for (const auto &point : contour) {
          // check if point is on the border
          if ((point.x == 0 || point.x == width - 1) &&
              (point.y == 0 || point.y == height - 1)) {
            isBorder = true;
          } else {
            isBorder = false;
            break;
          }
        }
        if (isBorder) {
          std::cout << "│\t\t☒ Contour is a border" << std::endl;
          continue;
        }
      }

      // Check if contour area is greater than a threshold (adjust this
      // threshold as needed)
      if (area > 30) { // Example threshold value
        // Approximate the contour to a polygon
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contour, approx, 0.03 * cv::arcLength(contour, true),
                         true);

        // draw this to imageCopy2
        // cv::drawContours(imageCopy2, contour, -1, cv::Scalar(0, 0, 255), 1);

        filtered_contours.push_back(contour);
        std::cout << "│\t\t☑ Useful (area " << area << ")" << std::endl;

        // Check if the contour is not entirely rectangular
        // if (approx.size() != 4) {
        //   filtered_contours.push_back(contour);
        //   std::cout << "│\t\t☑ Useful" << std::endl;
        // } else {
        //   std::cout << "│\t\t☒ Contour is a rectangle" << std::endl;
        //   std::cout << "│\t\tPoints: " << std::endl;
        //   for (const auto &point : approx) {
        //     std::cout << "│\t\t\t" << point << std::endl;
        //   }
        // }
      } else {
        std::cout << "│\t\t☒ Area is small (" << area << ")" << std::endl;
      }
    }

    // print contours to stdout
    int num_contours = 0;
    for (const auto &contour : filtered_contours) {
      // std::cout << "Contour: " << std::endl;
      // for (const auto &point : contour) {
      //   std::cout << point << std::endl;
      // }
      num_contours++;
    }

    cv::drawContours(demo_character_contours, filtered_contours, -1,
                     cv::Scalar(0, 0, 255), 1);

    std::cout << "│\tNumber of filtered contours: " << num_contours
              << std::endl;
    std::cout << "│\tAspect Ratio: " << aspectRatio << std::endl;
    std::cout << "│\tArea: " << area << std::endl;
    // std::cout << "│\tArea / Perimeter: "
    //           << area / cv::arcLength(contours[1], true) << std::endl;

    // cv::imshow("Component " + std::to_string(i), thresholded);
    // cv::imshow("Component contours" + std::to_string(i),
    // demo_character_contours);
    // cv::waitKey(0);

    // Draw bounding box around each component (for visualization)
    cv::rectangle(image, bbox, cv::Scalar(0, 0, 255), 1, cv::LINE_4);
  }

  // Display the result
  cv::imshow("Image with Characters", image);
  cv::waitKey(0);
  //
  // write to "output.txt"
  FILE *outputFile = fopen("output.txt", "w");
  fprintf(outputFile, "%s", recognizedText.c_str());
  fclose(outputFile);

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
