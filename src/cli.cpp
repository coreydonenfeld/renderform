// CLI entry-point
// Uses the public API via the CLI

#include "formula.hpp"
#include "modules/recognizer.hpp"
#include "renderform.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  // Current expected usage:
  // ./renderform <input_file>
  if (argc == 2) {
    std::string input_file = argv[1];
    Renderform::Formula output = Renderform::parseImage(input_file);
    std::cout << std::endl << "Parsed formula: " << std::endl;
    output.print();
    std::cout << std::endl;
    return 0;
  }

  std::cout << "Training k-NN model..." << std::endl;
  Renderform::Recognizer recognizer;
  return 0;

  std::string user_input;
  bool running = true;

  while (running) {
    std::cout << "Detect formula from image or enter formula manually? "
                 "(image/manual): ";
    std::cin >> user_input;
    if (user_input == "image") {
      std::string user_input_image;
      while (running) {
        std::cout << "Enter the image file ('exit' or 'q' to exit): ";
        std::cin >> user_input_image;
        if (user_input_image == "exit" || user_input_image == "q") {
          break;
        }
        Renderform::Formula output = Renderform::parseImage(user_input_image);
        std::cout << std::endl << "Parsed formula: " << std::endl;
        output.print();
        std::cout << std::endl;
      }

    } else if (user_input == "manual") {
      std::string user_formula;
      while (running) {
        std::cout << "Enter the formula ('exit' or 'q' to exit): ";
        std::cin >> user_formula;
        if (user_formula == "exit" || user_formula == "q") {
          break;
        }
        Renderform::Formula parsed_formula{user_formula,
                                           Renderform::FormType::EQUATION};
        std::cout << std::endl << "Parsed formula: " << std::endl;
        parsed_formula.print();
        std::cout << std::endl;
      }
    } else {
      std::cout << "Invalid input. Please try again." << std::endl;
      continue;
    }
  }

  return 0;
}
