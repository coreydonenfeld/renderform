// CLI entry-point
// Uses the public API via the CLI
//

#include "formula.hpp"
#include "renderform.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {

  // Current expected usage:
  // ./renderform <input_file>
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }

  std::string input_file = argv[1];
  std::string user_formula;

  // loop until user types "exit"

  while (true) {
    std::cout << "Enter the formula: ";
    std::cin >> user_formula;
    if (user_formula == "exit") {
      break;
    }
    Renderform::Formula parsed_formula{user_formula,
                                       Renderform::FormType::EQUATION};
    std::cout << std::endl << "Parsed formula: " << std::endl;
    parsed_formula.print();
    std::cout << std::endl;
  }

  // Renderform::Formula output = Renderform::parse(input_file);

  // std::cout << "Input file: " << input_file << std::endl;

  // cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
  // if (image.empty()) {
  //   std::cerr << "Error: <input_image> not found or could not be read."
  //             << std::endl;
  //   exit(EXIT_FAILURE);
  // }

  return 0;
}
