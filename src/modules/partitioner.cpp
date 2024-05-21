#include "partitioner.hpp"
#include "character.hpp"
#include <cmath>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

Renderform::Partitioner::Partitioner(cv::Mat *image)
    : source_binary_image(image) {}

Renderform::Partitioner::~Partitioner() {}

double Renderform::Partitioner::getArea(cv::Rect bbox) {
  return bbox.width * bbox.height;
}

cv::Rect Renderform::Partitioner::getLargestCharacterBoundingBox() {
  return this->largest_character_bbox;
}

cv::Mat resizeAndPad(const cv::Mat &src, int targetWidth, int targetHeight,
                     int paddingWidth, int paddingHeight, cv::Rect &roi) {
  if (src.empty()) {
    std::cerr << "Input image is empty." << std::endl;
    return cv::Mat();
  }

  int originalWidth = src.cols;
  int originalHeight = src.rows;

  // Calculate the aspect ratio
  double aspectRatio = static_cast<double>(originalWidth) / originalHeight;

  // Adjust target dimensions by subtracting the padding
  int adjustedWidth = targetWidth - 2 * paddingWidth;
  int adjustedHeight = targetHeight - 2 * paddingHeight;

  if (adjustedWidth <= 0 || adjustedHeight <= 0) {
    std::cerr << "Adjusted dimensions are too small." << std::endl;
    return cv::Mat();
  }

  int newWidth, newHeight;

  // Determine the new dimensions while preserving the aspect ratio
  if (originalWidth > originalHeight) {
    newWidth = adjustedWidth;
    newHeight = static_cast<int>(adjustedWidth / aspectRatio);
  } else {
    newHeight = adjustedHeight;
    newWidth = static_cast<int>(adjustedHeight * aspectRatio);
  }

  if (newWidth > adjustedWidth) {
    newWidth = adjustedWidth;
    newHeight = static_cast<int>(adjustedWidth / aspectRatio);
  }

  if (newHeight > adjustedHeight) {
    newHeight = adjustedHeight;
    newWidth = static_cast<int>(adjustedHeight * aspectRatio);
  }

  // Resize the image
  cv::Mat resizedImage;
  cv::resize(src, resizedImage, cv::Size(newWidth, newHeight), 0, 0,
             cv::INTER_AREA);

  // Create a new image with the target dimensions, filled with white pixels
  cv::Mat paddedImage(targetHeight, targetWidth, src.type(),
                      cv::Scalar(255, 255, 255));

  // Calculate the position to place the resized image (centered with padding)
  int xOffset = paddingWidth + (adjustedWidth - newWidth) / 2;
  int yOffset = paddingHeight + (adjustedHeight - newHeight) / 2;

  cv::Rect new_roi(xOffset, yOffset, newWidth, newHeight);
  roi = new_roi;

  // Place the resized image in the center of the new image
  resizedImage.copyTo(
      paddedImage(cv::Rect(xOffset, yOffset, newWidth, newHeight)));

  return paddedImage;
}

void Renderform::Partitioner::process() {
  /* Step 1: Connected component labeling on binary image. */
  cv::Mat labels, stats, centroids;
  int num_labels = cv::connectedComponentsWithStats(
      *(this->source_binary_image), labels, stats, centroids);
  this->num_characters = num_labels;

  /* Step 2: Create character objects for each connected component (that is
     sufficiently large). */
  for (int i = 1; i < this->num_characters; i++) {
    /* Get the bounding box in relation to the original image. */
    cv::Rect bounding_box_image(stats.at<int>(i, cv::CC_STAT_LEFT),
                                stats.at<int>(i, cv::CC_STAT_TOP),
                                stats.at<int>(i, cv::CC_STAT_WIDTH),
                                stats.at<int>(i, cv::CC_STAT_HEIGHT));

    if (getArea(bounding_box_image) > getArea(this->largest_character_bbox)) {
      this->largest_character_bbox = bounding_box_image;
    }

    /* Create an image containing only the character connected component. */
    cv::Mat character = cv::Mat::zeros(bounding_box_image.height,
                                       bounding_box_image.width, CV_8UC1);
    cv::Mat component = (labels(bounding_box_image) == i);
    component.copyTo(character(
        cv::Rect(0, 0, bounding_box_image.width, bounding_box_image.height)));

    /* Invert the image so that the character is black and the background is
       white (Tesseract does better with black text). */
    cv::bitwise_not(character, character);

    /* Avoid small components: they are likely noise. */
    if (cv::countNonZero(character) < 5) {
      std::cout << "Warning: Character too small" << std::endl;
      continue;
    }

    // if (bounding_box_image.height > 10 * height ||
    //     bounding_box_image.width > 10 * width) {
    //   std::cout << "Warning: Character too large" << std::endl;
    //   continue;
    // }

    /* Get the bounding box of the character in relation to the small character
       only padded image. */
    cv::Rect bounding_box_normalized;
    cv::Mat character_normalized = resizeAndPad(
        character, Renderform::Character::getWidth(),
        Renderform::Character::getHeight(), Renderform::Character::PADDING,
        Renderform::Character::PADDING, bounding_box_normalized);

    std::cout << "normalized bunding box: " << bounding_box_normalized
              << std::endl;

    // Strenghten the borders
    cv::Mat structuring_element =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(character_normalized, character_normalized, structuring_element,
              cv::Point(-1, -1), 1);

    cv::Point center(centroids.at<double>(i, 0), centroids.at<double>(i, 1));
    Renderform::Character character_object{character_normalized,
                                           bounding_box_image, center};
    character_object.setNormalizedBoundingBox(bounding_box_normalized);
    this->addCharacter(character_object);
  }
}

cv::Mat Renderform::Partitioner::processComponent(cv::Mat component) {
  return component;
}

double calculateRectangleLikelihood(const std::vector<cv::Point> &contour,
                                    double epsilon = 0.02) {
  // Approximate contour with accuracy proportional to the contour perimeter
  std::vector<cv::Point> approx;
  double peri = cv::arcLength(contour, true);
  cv::approxPolyDP(contour, approx, epsilon * peri, true);

  // Initialize score variables
  double score = 0.0;
  double max_score = 4.0;

  std::cout << "Approximated contour size: " << approx.size() << std::endl;

  // Check the number of vertices
  if (approx.size() == 4) {
    score += 1.0;
  } else if (approx.size() > 4) {
    score += 1.0 - std::min(static_cast<double>(approx.size() - 4) / 4.0, 1.0);
  }

  // Check if the approximated contour is convex
  if (cv::isContourConvex(approx)) {
    score += 1.0;
  }

  // Check the aspect ratio of the bounding box
  cv::Rect boundingBox = cv::boundingRect(approx);
  double aspectRatio =
      static_cast<double>(boundingBox.width) / boundingBox.height;
  if (aspectRatio > 1.0) {
    aspectRatio = 1.0 / aspectRatio;
  }
  score += aspectRatio; // This adds a value between 0 and 1

  // Check the angles
  double angleScore = 0.0;
  for (int i = 0; i < 4; i++) {
    double angle =
        std::fabs(cv::fastAtan2(approx[i].y - approx[(i + 1) % 4].y,
                                approx[i].x - approx[(i + 1) % 4].x) -
                  cv::fastAtan2(approx[(i + 2) % 4].y - approx[(i + 1) % 4].y,
                                approx[(i + 2) % 4].x - approx[(i + 1) % 4].x));
    if (angle > 65 && angle < 105) {
      angleScore += 1.0;
    }
  }
  score += (angleScore / 4.0); // Average angle score

  // Normalize the score to be between 0 and 1
  return score / max_score;
}

Renderform::Characters Renderform::Partitioner::getCharacters() {

  // Sort characters by y coordinates (first step to group characters by lines)
  std::sort(this->characters.begin(), this->characters.end(),
            [](const Renderform::Character &a, const Renderform::Character &b) {
              return a.getCenter().y < b.getCenter().y;
            });

  // Find threshold to group characters by lines
  double threshold = this->getLargestCharacterBoundingBox().height * 0.25;

  // start group and end group once y coordinates changes by a certain threshold
  Renderform::LineCharacters group;
  int prev_y = this->characters[0].getCenter().y;
  int line_number = 0;
  for (auto &character : this->characters) {
    if (threshold < std::abs(character.getCenter().y -
                             prev_y)) { // @todo add condition if largest x
                                        // distance is 3x as much -- new line
      /* Reset group and add previous line to groups */
      this->lines.push_back(group);
      group.clear();
    }

    // // Get contours of character and print them in red
    // cv::Mat character_contours = character.getComponent().clone();
    // cv::Mat structuring_element =
    //     cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
    // cv::erode(character_contours, character_contours, structuring_element,
    //           cv::Point(-1, -1), 1);
    // cv::threshold(character_contours, character_contours, 30, 255,
    //               cv::THRESH_BINARY | cv::THRESH_OTSU);
    // std::vector<std::vector<cv::Point>> contours;
    // cv::findContours(character_contours, contours, cv::RETR_TREE,
    //                  cv::CHAIN_APPROX_NONE);

    // cv::Mat character_contours_color;
    // cv::cvtColor(character_contours, character_contours_color,
    //              cv::COLOR_GRAY2BGR);

    // for (int i = 1; i < contours.size(); i++) {
    //   cv::drawContours(character_contours_color, contours, i,
    //                    cv::Scalar(0, 0, 255), 1, cv::LINE_8);
    // }

    // Add character to group sorted by x coordinates
    group.insert({character.getCenter().x, character});

    // group.push_back(character);
    prev_y = character.getCenter().y;

    line_number++;
  }
  this->lines.push_back(group);

  for (auto &line : this->lines) {
    Renderform::Character previous_character;
    auto previous_character_it = line.begin();
    int previous_character_type = -1;
    for (auto it = line.begin(); it != line.end(); ++it) {
      auto &[x, character] = *it;
      std::cout << character.getBoundingBox().area() << std::endl;

      // get width
      int width = character.getBoundingBox().width;
      int height = character.getBoundingBox().height;
      double aspect_ratio = static_cast<double>(width) / height;
      std::cout << "Aspect ratio: " << aspect_ratio << std::endl;
      // cv::imshow("Character", character.getComponent());
      // cv::waitKey(0);

      if (previous_character_type == 1 && aspect_ratio > 1.25) {
        // std::cout << "previous character y center: "
        //           << previous_character.getCenter().y << std::endl;
        // std::cout << "previous character x center: "
        //           << previous_character.getCenter().x << std::endl;
        // std::cout << "current character y center: " <<
        // character.getCenter().y
        //           << std::endl;
        // std::cout << "current character x center: " <<
        // character.getCenter().x
        //           << std::endl;
        // Calculate distance between characters
        double distance =
            cv::norm(previous_character.getCenter() - character.getCenter());
        // std::cout << getLargestCharacterBoundingBox() << std::endl;
        // std::cout << "Distance between characters: " << distance <<
        // std::endl;

        if (distance > getLargestCharacterBoundingBox().height) {
          continue;
        }
        // std::cout
        //     << "Previous character is a wide rectangle and this one is too"
        //     << std::endl;

        // std::cout << "Previous character: "
        //           << previous_character.getBoundingBox() << std::endl;
        // std::cout << "Current character: " << character.getBoundingBox()
        //           << std::endl;

        // join the bounding boxes
        // get the two characters

        cv::Rect combined_bounding_box =
            previous_character.getBoundingBox() | character.getBoundingBox();
        std::cout << "Combined bounding box: " << combined_bounding_box
                  << std::endl;
        cv::Mat combined_components =
            (*(this->source_binary_image))(combined_bounding_box);

        cv::bitwise_not(combined_components, combined_components);

        /* Avoid small components: they are likely noise. */
        if (cv::countNonZero(combined_components) < 5) {
          std::cout << "Warning: Character too small" << std::endl;
          continue;
        }

        /* Get the bounding box of the character in relation to the small
           character only padded image. */
        cv::Rect bounding_box_normalized;
        cv::Mat character_normalized = resizeAndPad(
            combined_components, Renderform::Character::getWidth(),
            Renderform::Character::getHeight(), Renderform::Character::PADDING,
            Renderform::Character::PADDING, bounding_box_normalized);

        std::cout << "normalized bunding box: " << bounding_box_normalized
                  << std::endl;

        // Strenghten the borders
        cv::Mat structuring_element =
            cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::erode(character_normalized, character_normalized,
                  structuring_element, cv::Point(-1, -1), 1);

        cv::Point center = cv::Point(
            combined_bounding_box.x + combined_bounding_box.width / 2,
            combined_bounding_box.y + combined_bounding_box.height / 2);

        Renderform::Character combined_character(character_normalized,
                                                 combined_bounding_box, center);
        combined_character.setNormalizedBoundingBox(bounding_box_normalized);
        combined_character.setLabelDetermined("=");

        // cv::imshow("Combined character", combined_character.getComponent());
        // cv::waitKey(0);

        line.erase(previous_character_it);
        line.erase(it++);
        line.insert({center.x, combined_character});

        // add combined character in current position

        previous_character_type = 0;
      } else {

        // aspect ratio > 2 = wide rectangle

        if (aspect_ratio > 1.25) {
          previous_character_type = 1;
          previous_character = character;
          previous_character_it = it;
        } else {
          previous_character_type = 0;
        }
      }
    }
    std::cout << std::endl;
  }

  // Get each line
  // for (auto &line : this->lines) {
  //   std::cout << "Line " << line.first << " has " << line.second.size()
  //             << " characters" << std::endl;
  // }

  // sort by x coordinates
  // int group_index = 0;
  // for (auto &group : groups) {
  //   group_index++;
  //   std::sort(
  //       group.begin(), group.end(),
  //       [](const Renderform::Character &a, const Renderform::Character &b)
  //       {
  //         return a.getCenter().x < b.getCenter().x;
  //       });
  //   std::cout << "Group " << group_index << " has " << group.size()
  //             << " characters" << std::endl;
  //   int min_x = 99999;
  //   int max_x = 0;
  //   int min_y = 99999;
  //   int max_y = 0;
  //   for (auto &character : group) {
  //     std::cout << character.getCenter().x << " " <<
  //     character.getCenter().y
  //               << std::endl;
  //     if (character.getCenter().x < min_x) {
  //       min_x = character.getTopLeft().x;
  //     }
  //     if (character.getCenter().x > max_x) {
  //       max_x = character.getTopRight().x;
  //     }
  //     if (character.getCenter().y < min_y) {
  //       min_y = character.getBottomRight().y;
  //     }
  //     if (character.getCenter().y > max_y) {
  //       max_y = character.getTopRight().y;
  //     }
  //   }
  //   std::cout << "Group " << group_index << " has min_x: " << min_x
  //             << " max_x: " << max_x << " min_y: " << min_y
  //             << " max_y: " << max_y << std::endl;
  //   // add rectangle to source_binary_image
  //   cv::rectangle(*(this->source_binary_image), cv::Point(min_x, min_y),
  //                 cv::Point(max_x, max_y), cv::Scalar(255, 255, 255), 2);
  //   cv::imshow("Group " + std::to_string(group_index),
  //              *(this->source_binary_image));
  //   cv::waitKey(0);
  // }

  /*
  sort by y and x coordinates
  */
  // std::sort(this->characters.begin(), this->characters.end(),
  //           [](const Renderform::Character &a, const Renderform::Character
  //           &b) {
  //             return a.getCenter().x < b.getCenter().x;
  //           });

  return this->characters;
}

Renderform::Lines Renderform::Partitioner::getLines() { return this->lines; }

void Renderform::Partitioner::addCharacter(Renderform::Character character) {
  this->characters.push_back(character);
}
