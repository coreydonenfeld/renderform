#ifndef RENDERFORM_PARTITIONER_H
#define RENDERFORM_PARTITIONER_H

#include "character.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

namespace Renderform {

typedef std::vector<Character> Characters;

class Partitioner {
public:
  Partitioner(cv::Mat *image);
  ~Partitioner();

  void process();

  cv::Mat processComponent(cv::Mat component);

  Characters getCharacters();

private:
  cv::Mat *source_binary_image;
  Characters characters;
  int num_characters;

  void addCharacter(Character character);
};
} // namespace Renderform

#endif // RENDERFORM_PARTITIONER_H
