#include "renderform.hpp"
#include "formula.hpp"
#include "modules/cleaner.hpp"
#include "modules/partitioner.hpp"
#include "modules/recognizer.hpp"
#include <opencv2/highgui.hpp>

// @todo maybe look into this:
// https://github.com/opencv/opencv/blob/master/samples/dnn/text_detection.cpp

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

  // cv::imshow("Binarized Image", image_binarized);
  // cv::waitKey(0);

  Renderform::Partitioner partitioner{&image_binarized};
  partitioner.process();
  Renderform::Characters partitions = partitioner.getCharacters();

  std::cout << "Largest character bbox"
            << partitioner.getLargestCharacterBoundingBox() << std::endl;

  Renderform::Recognizer ocr;

  cv::Mat image_display = image_src.clone();

  int line_number = 0;
  for (const auto &line : partitioner.getLines()) {
    int character_number = 0;
    cv::Point top_left{INT_MAX, INT_MAX};
    cv::Point bottom_right{0, 0};
    for (const auto &[x, character] : line) {
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
      // std::cout << "Character " << line_number << ":" << character_number
      //           << " guess: " << character_text << std::endl;
      cv::putText(
          image_display,
          std::to_string(line_number) + ":" + std::to_string(character_number) +
              " " + character_text,
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
    // ocr.recognize(line);
    std::cout << "Line " << line_number << " has " << character_number
              << " characters" << std::endl;

    line_number++;
  }

  cv::imshow("Image display with rects", image_display);
  cv::waitKey(0);

  // std::cout << "Number of characters: " << partitions.size() << std::endl;
  // for (int i = 0; i < partitions.size(); i++) {
  //   std::cout << "Character " << i << std::endl;
  //   cv::imshow("Character " + std::to_string(i),
  //   partitions[i].getComponent()); cv::waitKey(0); std::cout << "result: " <<
  //   ocr.recognize(partitions[i]) << std::endl;
  // }

  cv::destroyAllWindows();
  cv::waitKey(1);

  return parsed_formula;
}

Renderform::Formula Renderform::parseText(const std::string &text) {
  Renderform::Formula parsed_formula{text, Renderform::FormType::EQUATION};
  return parsed_formula;
}
