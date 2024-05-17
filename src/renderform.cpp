#include "renderform.hpp"
#include "modules/cleaner.hpp"
#include "modules/partitioner.hpp"
#include "modules/recognizer.hpp"

Renderform::Formula Renderform::parse(const std::string &image_path) {
  Renderform::Formula parsed_formula;

  cv::Mat image_src = cv::imread(image_path, cv::IMREAD_COLOR);
  if (image_src.empty()) {
    std::cerr << "Error: <image> not found or could not be read." << std::endl;
    return parsed_formula;
  }

  Renderform::Cleaner preprocessor{&image_src};
  preprocessor.process();
  cv::Mat image_binarized = preprocessor.getProcessedImage();

  cv::imshow("Binarized Image", image_binarized);
  cv::waitKey(0);

  Renderform::Partitioner partitioner{&image_binarized};
  partitioner.process();
  Renderform::Characters partitions = partitioner.getCharacters();

  Renderform::Recognizer ocr;

  std::cout << "Number of characters: " << partitions.size() << std::endl;
  for (int i = 0; i < partitions.size(); i++) {
    std::cout << "Character " << i << std::endl;
    cv::imshow("Character " + std::to_string(i), partitions[i].getComponent());
    cv::waitKey(0);
    std::cout << "result: " << ocr.recognize(partitions[i]) << std::endl;
  }

  return parsed_formula;
}
