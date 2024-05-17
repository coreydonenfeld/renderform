#include "partitioner.hpp"
#include "character.hpp"

Renderform::Partitioner::Partitioner(cv::Mat *image)
    : source_binary_image(image) {}

Renderform::Partitioner::~Partitioner() {}

void Renderform::Partitioner::process() {
  const int PADDING = 9;

  // Apply connected component labeling
  cv::Mat labels, stats, centroids;
  int num_labels = cv::connectedComponentsWithStats(
      *(this->source_binary_image), labels, stats, centroids);

  this->num_characters = num_labels;

  for (int i = 1; i < this->num_characters; i++) {
    // std::cout << "╭ Determining <Object " << i << "> Character" << std::endl;

    cv::Rect bbox(stats.at<int>(i, cv::CC_STAT_LEFT),
                  stats.at<int>(i, cv::CC_STAT_TOP),
                  stats.at<int>(i, cv::CC_STAT_WIDTH),
                  stats.at<int>(i, cv::CC_STAT_HEIGHT));

    // create new tmp image that is the bounding box of the connected component
    // + padding to draw the shape in the center
    cv::Mat character_padded = cv::Mat::zeros(
        bbox.height + 2 * PADDING, bbox.width + 2 * PADDING, CV_8UC1);
    cv::Mat component = (labels(bbox) == i);
    // add the connected component to the tmp image
    component.copyTo(
        character_padded(cv::Rect(PADDING, PADDING, bbox.width, bbox.height)));
    // Invert the connected component
    cv::bitwise_not(character_padded, character_padded);

    // Strenghten the borders
    cv::Mat structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
    cv::dilate(character_padded, character_padded, structuring_element, cv::Point(-1, -1),
      2);

    // De-pixelate the character
    cv::GaussianBlur(character_padded, character_padded, cv::Size(3, 3), 0,
                     0);

    // // Threshold and binarize the original image
    // // (Currently assumes no overlap - this might need to be adjusted and
    // fixed
    // // to refind ONLY the connected component in the original image)
    // cv::Mat character_thresholded;
    // cv::threshold(character_padded, character_thresholded, 0, 255,
    //               cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    cv::Point center(centroids.at<double>(i, 0), centroids.at<double>(i, 1));
    Renderform::Character character_object{character_padded, bbox, center};

    this->addCharacter(character_object);

    continue;

    // cv::erode(character_thresholded, character_thresholded, cv::Mat(),
    //           cv::Point(-1, -1), 1);

    // // get contours of the connected component
    // std::vector<std::vector<cv::Point>> character_contours;
    // cv::findContours(character_thresholded, character_contours,
    // cv::RETR_TREE,
    //                  cv::CHAIN_APPROX_SIMPLE);

    // if (character_contours.size() == 0) {
    //   std::cout << "No contours found for component " << i << std::endl;
    //   continue;
    // }

    // std::cout << "Number of contours: " << character_contours.size()
    //           << std::endl;

    // cv::Mat character_demo;
    // // get contour of the shape not the bounding box
    // // make thresholded image rgb to draw contours
    // cv::cvtColor(character_thresholded, character_demo, cv::COLOR_GRAY2BGR);

    // // cv::drawContours(character_demo, character_contours, 1,
    // //                  cv::Scalar(0, 255, 0), -1);

    // // cv::drawContours(character_demo, character_contours, 2,
    // //                  cv::Scalar(0, 0, 255), 1);

    // cv::drawContours(character_thresholded, character_contours, 1,
    //                  cv::Scalar(0), -1);

    // for (int j = 2; j < character_contours.size(); j++) {
    //   cv::drawContours(character_thresholded, character_contours, j,
    //                    cv::Scalar(0), 1);
    // }

    // De-pixelate the character
    // cv::GaussianBlur(character_thresholded, character_thresholded,
    //                  cv::Size(13, 13), 0, 0);
    // cv::erode(character_thresholded, character_thresholded, cv::Mat(),
    //           cv::Point(-1, -1), 1);

    // fill in outline to get a solid shape
    // Invert thresholded image
    // cv::bitwise_not(character_thresholded, character_thresholded);

    // // Apply dilation and erosion to the thresholded image
    // int dilation_size = 1;
    // int dilation_type = cv::MORPH_ERODE;
    // cv::Mat element = getStructuringElement(
    //     dilation_type, cv::Size(2 * dilation_size + 1, 2 * dilation_size +
    //     1), cv::Point(dilation_size, dilation_size));

    // // cv::erode(character_thresholded, character_thresholded, element,
    // //           cv::Point(-1, -1), 1);
    // // cv::dilate(character_thresholded, character_thresholded, element,
    // //            cv::Point(-1, -1), 1);

    // // cv::imshow("Character Bounding Box " + std::to_string(i),
    // //            character_thresholded);
    // // cv::waitKey(0);

    // // Assuming the connected component is a single character, we can use the
    // // properties of the character and do some non-OCR based determination
    // below
    // // is AI generated and most definitely not accurate but is a good
    // starting
    // // point for example, if the height is greater than the width, it is
    // likely
    // // a 1, 4, 7, or 9 if the height is less than the width, it is likely a
    // 0,
    // // 2, 3, 5, 6, or 8 if the height is equal to the width, it is likely a
    // 1,
    // // 4, 7, or 9 if the height is less than 1.5 times the width, it is
    // likely a
    // // 1, 4, 7, or 9 if the height is greater than 1.5 times the width, it is
    // // likely a 0, 2, 3, 5, 6, or 8 if the height is greater than 2 times the
    // // width, it is likely a 0, 2, 3, 5, 6, or 8 if the height is greater
    // than 3
    // // times the width, it is likely a 0, 2, 3, 5, 6, or 8
    // //
    // // Back to human:
    // // if it is a long stick with a small hat, it is likely a 1
    // //

    // // Calculate properties of the thresholded image
    // // a. Aspect ratio
    // // b. Area
    // // c. Height
    // // d. Width
    // // e. Number of holes
    // // f. Number of edges
    // // g. Number of corners
    // // h. Number of intersections
    // // i. Number of loops
    // // j. Number of endpoints
    // // k. Circularity
    // // l. straightness
    // // m. thickness
    // // n. slant
    // //
    // std::cout << "├┄┄ Object Properties" << std::endl;

    // double width = bbox.width;
    // double height = bbox.height;
    // double aspectRatio = width / height;
    // double area = cv::countNonZero(character_thresholded);

    // std::vector<std::vector<cv::Point>> contours;
    // cv::findContours(character_thresholded, contours, cv::RETR_CCOMP,
    //                  cv::CHAIN_APPROX_SIMPLE);

    // std::cout << "│\tHeight: " << height << std::endl;
    // std::cout << "│\tWidth: " << width << std::endl;

    // std::cout << "│\tNumber of raw contours: " << contours.size() <<
    // std::endl;

    // // draw contours to visualize
    // // make copy of input image, bound it with the bounding box and draw
    // // contours
    // cv::Mat demo_character_contours = character.clone();
    // // change character to color for visualization
    // cv::cvtColor(demo_character_contours, demo_character_contours,
    //              cv::COLOR_GRAY2BGR);
    // // add padding for demo purposes
    // cv::rectangle(demo_character_contours, bbox, cv::Scalar(0, 255, 0), 2);
    // cv::drawContours(demo_character_contours, contours, -1,
    //                  cv::Scalar(0, 0, 255), 2);

    // std::vector<std::vector<cv::Point>> filtered_contours;
    // int contour_i = 0;
    // for (const auto &contour : contours) {
    //   contour_i++;

    //   std::cout << "│\t╰┄┄ Contour " << contour_i << " (size " <<
    //   contour.size()
    //             << ")" << std::endl;

    //   // Compute contour area
    //   double area = cv::contourArea(contour);

    //   // if 4 points and the x is 0 or width and y is 0 or height, it is the
    //   // border and should be ignored
    //   if (contour.size() == 4) {
    //     bool isBorder = false;
    //     std::cout << "│\t\tContour is a rectangle (FIRST CHECK)" <<
    //     std::endl; for (const auto &point : contour) {
    //       // check if point is on the border
    //       if ((point.x == 0 || point.x == width - 1) &&
    //           (point.y == 0 || point.y == height - 1)) {
    //         isBorder = true;
    //       } else {
    //         isBorder = false;
    //         break;
    //       }
    //     }
    //     if (isBorder) {
    //       std::cout << "│\t\t☒ Contour is a border" << std::endl;
    //       continue;
    //     }
    //   }

    //   // Check if contour area is greater than a threshold (adjust this
    //   // threshold as needed)
    //   if (area > 30) { // Example threshold value
    //     // Approximate the contour to a polygon
    //     std::vector<cv::Point> approx;
    //     cv::approxPolyDP(contour, approx, 0.03 * cv::arcLength(contour,
    //     true),
    //                      true);

    //     // draw this to imageCopy2
    //     // cv::drawContours(imageCopy2, contour, -1, cv::Scalar(0, 0, 255),
    //     1);

    //     filtered_contours.push_back(contour);
    //     std::cout << "│\t\t☑ Useful (area " << area << ")" << std::endl;

    //     // Check if the contour is not entirely rectangular
    //     // if (approx.size() != 4) {
    //     //   filtered_contours.push_back(contour);
    //     //   std::cout << "│\t\t☑ Useful" << std::endl;
    //     // } else {
    //     //   std::cout << "│\t\t☒ Contour is a rectangle" << std::endl;
    //     //   std::cout << "│\t\tPoints: " << std::endl;
    //     //   for (const auto &point : approx) {
    //     //     std::cout << "│\t\t\t" << point << std::endl;
    //     //   }
    //     // }
    //   } else {
    //     std::cout << "│\t\t☒ Area is small (" << area << ")" << std::endl;
    //   }
    // }

    // // print contours to stdout
    // int num_contours = 0;
    // for (const auto &contour : filtered_contours) {
    //   // std::cout << "Contour: " << std::endl;
    //   // for (const auto &point : contour) {
    //   //   std::cout << point << std::endl;
    //   // }
    //   num_contours++;
    // }

    // cv::drawContours(demo_character_contours, filtered_contours, -1,
    //                  cv::Scalar(0, 0, 255), 1);

    // std::cout << "│\tNumber of filtered contours: " << num_contours
    //           << std::endl;
    // std::cout << "│\tAspect Ratio: " << aspectRatio << std::endl;
    // std::cout << "│\tArea: " << area << std::endl;
    // std::cout << "│\tArea / Perimeter: "
    //           << area / cv::arcLength(contours[1], true) << std::endl;

    // cv::imshow("Component " + std::to_string(i), thresholded);
    // cv::imshow("Component contours" + std::to_string(i),
    // demo_character_contours);
    // cv::waitKey(0);

    // Draw bounding box around each component (for visualization)
    // cv::rectangle(image, bbox, cv::Scalar(0, 0, 255), 1, cv::LINE_4);
  }
}

cv::Mat Renderform::Partitioner::processComponent(cv::Mat component) {
  return component;
}

Renderform::Characters Renderform::Partitioner::getCharacters() {
  return this->characters;
}

void Renderform::Partitioner::addCharacter(Renderform::Character character) {
  this->characters.push_back(character);
}
