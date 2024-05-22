#include "renderform.hpp"
#include "formula.hpp"
#include "modules/cleaner.hpp"
#include "modules/partitioner.hpp"
#include "modules/recognizer.hpp"

Renderform::Formula Renderform::parseImage(const std::string &image_path) {
  Renderform::Formula parsed_formula;

  cv::Mat image_src = cv::imread(image_path, cv::IMREAD_COLOR);
  if (image_src.empty()) {
    std::cerr << "Error: <image> not found or could not be read." << std::endl;
    return parsed_formula;
  }

  Renderform::Cleaner preprocessor{&image_src};
  preprocessor.process();
  cv::Mat image_binarized = preprocessor.getProcessedImage();

  Renderform::Partitioner partitioner{&image_binarized};
  partitioner.process();
  Renderform::Characters partitions = partitioner.getCharacters();

  Renderform::Recognizer ocr;

  std::vector<std::string> lines_text;

  cv::Mat image_display = image_src.clone();

  int line_number = 0;
  for (auto &line : partitioner.getLines()) {
    std::string line_text;
    int character_number = 0;
    cv::Point top_left{INT_MAX, INT_MAX};
    cv::Point bottom_right{0, 0};
    for (auto &[x, character] : line) {
      // Get top left and bottom right points of the bounding box
      top_left.x = std::min(top_left.x, character.getBoundingBox().x);
      top_left.y = std::min(top_left.y, character.getBoundingBox().y);
      bottom_right.x =
          std::max(bottom_right.x, character.getBoundingBox().x +
                                       character.getBoundingBox().width);
      bottom_right.y =
          std::max(bottom_right.y, character.getBoundingBox().y +
                                       character.getBoundingBox().height);

      // For visualization
      std::string character_text = ocr.recognize(character);
      std::cout << "Character " << line_number << ":" << character_number
                << "\n\tGuess from Knn: " << character_text
                << "\n\tGuess from Tesseract (confidence: "
                << character.getTesseractConfidence()
                << "%): " << character.getLabelPredictedTesseract()
                << std::endl;

      if (!character.getLabelDetermined().empty()) {
        line_text += character.getLabelDetermined();
      } else if (!character.getLabelPredictedTesseract().empty() &&
                 (character.getTesseractConfidence() >= 80 ||
                  character.getLabelPredictedTesseract() == "+")) {
        line_text += character.getLabelPredictedTesseract();
      } else {
        line_text += character_text;
      }

      // @todo Really small 0 or o or O should be treated as * (multiplication)

      cv::rectangle(
          image_display,
          cv::Point(character.getBoundingBox().x, character.getBoundingBox().y),
          cv::Point(
              character.getBoundingBox().x + character.getBoundingBox().width,
              character.getBoundingBox().y + character.getBoundingBox().height),
          cv::Scalar(0, 255, 0), 2);
      cv::putText(
          image_display,
          character.getLabelPredictedTesseract() + " " +
              character.getLabelPredictedKnn(),
          cv::Point(character.getBoundingBox().x, character.getBoundingBox().y),
          cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
      character_number++;
    }
    int line_color_val =
        (double)line_number / partitioner.getLines().size() * 255;
    cv::Scalar line_color =
        cv::Scalar(line_color_val % 255, (line_color_val + 128) % 255,
                   255 - line_color_val);
    cv::rectangle(image_display, top_left, bottom_right, line_color, 2);
    std::cout << "Line " << line_number << " has " << character_number
              << " characters" << std::endl;
    std::cout << "---" << std::endl;

    lines_text.push_back(line_text);
    line_number++;
  }

  std::cout << "Parsed formulas: " << std::endl;
  std::cout << "---" << std::endl;

  for (int i = 0; i < lines_text.size(); i++) {
    Renderform::Formula parsed_line(lines_text[i]);
    std::cout << "Line " << i << ": " << lines_text[i] << std::endl;
    std::cout << "\tLatex: " << parsed_line.getLaTeX() << std::endl;
  }

  cv::imshow("Image display with rects", image_display);
  cv::waitKey(0);

  cv::destroyAllWindows();
  cv::waitKey(1);

  return parsed_formula;
}

Renderform::Formula Renderform::parseText(const std::string &text) {
  Renderform::Formula parsed_formula{text, Renderform::FormType::EQUATION};
  return parsed_formula;
}
