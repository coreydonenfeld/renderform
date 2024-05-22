#ifndef RENDERFORM_PARTITIONER_H
#define RENDERFORM_PARTITIONER_H

#include "character.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

namespace Renderform {

typedef std::vector<Character> Characters;       // Unsorted
typedef std::map<int, Character> LineCharacters; // Sorted by x
typedef std::vector<LineCharacters> Lines;

struct Line {
  cv::Rect bounding_box;
  Characters characters;
};

class Partitioner {
public:
  Partitioner(cv::Mat *image);
  ~Partitioner();

  void process();

  cv::Mat processComponent(cv::Mat component);

  Characters getCharacters();
  Lines getLines();

  cv::Rect getLargestCharacterBoundingBox();

  static double getArea(cv::Rect bbox);

private:
  cv::Mat *source_binary_image;
  Characters characters;
  Lines lines;
  int num_characters;
  int num_lines;

  cv::Rect largest_character_bbox{0, 0, 0, 0};

  void addCharacter(Character character);
};
} // namespace Renderform

#endif // RENDERFORM_PARTITIONER_H
