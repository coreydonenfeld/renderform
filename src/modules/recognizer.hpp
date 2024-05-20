#ifndef RENDERFORM_RECOGNIZER_H
#define RENDERFORM_RECOGNIZER_H

#include "character.hpp"
#include <opencv2/opencv.hpp>
#include <string>
#include <tesseract/baseapi.h>
#include <utility>
#include <vector>

namespace Renderform {

struct RecognizerResult {
  std::string text;
  int confidence;

  cv::Rect bounding_box;
};

class Recognizer {
public:
  Recognizer(std::string language = "eng");
  ~Recognizer();

  tesseract::TessBaseAPI &TesseractAPI();

  std::string recognize(const Character &character);

  // std::string recognize(const Line &line);

private:
  tesseract::TessBaseAPI tesseract_api;
  std::string language;
  // std::vector<std::pair<std::string, std::string>> tesseract_variables;
};
} // namespace Renderform

#endif // RENDERFORM_RECOGNIZER_H
