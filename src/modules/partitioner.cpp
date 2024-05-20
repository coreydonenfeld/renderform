#include "partitioner.hpp"
#include "character.hpp"
#include <cmath>

Renderform::Partitioner::Partitioner(cv::Mat *image)
    : source_binary_image(image) {}

Renderform::Partitioner::~Partitioner() {}

double Renderform::Partitioner::getArea(cv::Rect bbox) {
  return bbox.width * bbox.height;
}

// double Renderform::Partitioner::getArea(cv::Mat image) {
//   return cv::countNonZero(image);
// }

cv::Rect Renderform::Partitioner::getLargestCharacterBoundingBox() {
  return this->largest_character_bbox;
}

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

    if (getArea(bbox) > getArea(this->largest_character_bbox)) {
      this->largest_character_bbox = bbox;
    }

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
    cv::Mat structuring_element =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
    cv::dilate(character_padded, character_padded, structuring_element,
               cv::Point(-1, -1), 2);

    // De-pixelate the character
    // cv::GaussianBlur(character_padded, character_padded, cv::Size(3, 3), 0,
    // 0);

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
  }
}

cv::Mat Renderform::Partitioner::processComponent(cv::Mat component) {
  return component;
}

Renderform::Characters Renderform::Partitioner::getCharacters() {

  // Sort characters by y coordinates (first step to group characters by lines)
  std::sort(this->characters.begin(), this->characters.end(),
            [](const Renderform::Character &a, const Renderform::Character &b) {
              return a.getCenter().y < b.getCenter().y;
            });

  // Find threshold to group characters by lines -- half of largest character
  // height
  double threshold = this->getLargestCharacterBoundingBox().height * 0.5;

  // start group and end group once y coordinates changes by a certain threshold
  Renderform::LineCharacters group;
  int prev_y = this->characters[0].getCenter().y;
  int line_number = 0;
  for (auto &character : this->characters) {
    if (threshold < std::abs(character.getCenter().y - prev_y)) {
      /* Reset group and add previous line to groups */
      this->lines.push_back(group);
      group.clear();
    }
    // Add character to group sorted by x coordinates
    group.insert({character.getCenter().x, character});

    // group.push_back(character);
    prev_y = character.getCenter().y;

    line_number++;
  }
  this->lines.push_back(group);

  std::cout << "Number of lines: " << this->lines.size() << std::endl;

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
  //       [](const Renderform::Character &a, const Renderform::Character &b) {
  //         return a.getCenter().x < b.getCenter().x;
  //       });
  //   std::cout << "Group " << group_index << " has " << group.size()
  //             << " characters" << std::endl;
  //   int min_x = 99999;
  //   int max_x = 0;
  //   int min_y = 99999;
  //   int max_y = 0;
  //   for (auto &character : group) {
  //     std::cout << character.getCenter().x << " " << character.getCenter().y
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
