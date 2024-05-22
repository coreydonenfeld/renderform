# Overview
Renderform is a handwritten mathematical formula recognition system that processes images of handwritten text and parses them into recognized mathematical formulas, which can be rendered in LaTeX. The system is designed to be user-friendly and accessible, providing a simple C++ API and CLI for users to interact with. Renderform is built using C++ and OpenCV, leveraging the k-Nearest Neighbors (k-NN) classifier for character recognition.

## Pipeline
### Image Pre-processing
- Convert image to grayscale
- Separate paper from miscellaneous objects in the background (if applicable)
- Remove noise
- Isolate text from paper
- Binary thresholding (digits are white, everything else should be black)
### Character Partitioning
- Segment text into individual characters (connected component labeling)
- Assign each character a bounding box (in the image world view)
- Normalize character size to 60x60, centering the character in the bounding box, preserving its aspect ratio, and padding 12 pixels on all sides (i.e., actual character max size is 36x36)
- Enhance character shape/edges with morphological operations (dilation, erosion)
### Line Segmentation
- Order all characters top-to-bottom, left-to-right
- Group characters into lines (formulas/expressions), ordered top-to-bottom based on the y-coordinate of the bounding box
- Assign each line a bounding box (in the image world view)
- Order characters within lines left-to-right
### Character Recognition
- Train k-NN classifier with labeled character data
- Classify each character using the k-NN classifier
- Map each classified character to its ASCII representation
### Equation Parsing & Formation
- Concatenate characters within each line to form equations
  - Handle special cases (e.g., fractions, exponents, square roots) by using character bounding boxes and relationships to other characters in the same line
- Parse equation using a custom parse tree (binary tree) data structure
  - Each node in the tree represents an operator or operand (character)
  - Each node has a left and right child, representing the left and right operands of the operator
### LaTeX Generation
- For each equation, generate LaTeX code based on the parsed equation tree
- In progress: Generate an image of the LaTeX-rendered equation (not yet implemented)

## Usage
### Dependencies
- OpenCV 4.5.3
- CMake 3.21.3
- C++17
- Tesseract OCR 4.1.1
- Leptonica 1.80.0
- LaTeX (optional, for rendering LaTeX code)
### Build from Source
Warning: Only tested on macOS 14.4.1. This project is still in development and may not work as expected. Please use at your own risk.
```bash
mkdir build
cd build
cmake ..
make
./renderform <path_to_image>
```
### API
```cpp
#include "renderform/renderform.h"

int main() {
  Renderform::Formula output = Renderform::parseImage(input_file);
  // Print to stdout
  output.print();
  // Get raw plain-text formula
  std::string formula = output.getFormulaRaw();
  // Get LaTeX formula
  std::string latex = output.getFormulaLaTeX();
  return 0;
}
```
### CLI
```bash
./renderform <path_to_image>
```

Successes

Failures

Limitations

## Character Recognition
### k-NN Classifier (OpenCV Implementation)
https://docs.opencv.org/4.9.0/d8/d4b/tutorial_py_knn_opencv.html

k-NN is a great approach for individual character recognition but gets very slow, expensive, and is not very scalable. It proved to work nicely for most expressions but failed occasionally, yielding jumbled incorrect results. The k-NN classifier is a great starting point for this project but is not the best solution for the final product. Perhaps in the future, it would be ideal to implement a more sophisticated model, such as a Convolutional Neural Network (CNN), to improve the performance and accuracy of the character recognition system.

### Tesseract OCR
https://tesseract-ocr.github.io/

Tesseract OCR is a great tool for character recognition but *not* for handwritten text. It particularly fails when the input contains math symbols and expressions. The pre-trained model for English is extremely impressive yet flawed for this project's specific purposes of parsing handwritten mathematical formulas. Tesseract seemed like a great option prior to beginning the project. Unfortunately, it yielded subpar, haphazard results. A stretch goal, beyond the scope of this project in its current state, would be to train Tesseract with custom data, potentially from the MNIST database, to improve its performance.

## Inspiration & References
https://blog.ayoungprogrammer.com/2013/01/equation-ocr-part-2-training-characters.html
https://github.com/floneum/floneum/blob/main/models/kalosm-ocr/examples/ocr.rs
https://github.com/stevenobadja/math_object_detection?tab=readme-ov-file
https://arxiv.org/pdf/1003.5898
https://groups.google.com/g/tesseract-ocr/c/lvr_xaSuSe0
https://www.youtube.com/watch?v=a5oeEhTf6_M


- set bounding boxes for each line, which will be an equation
- within the line, bound each character. map it to a ASCII character
- need to note position and size of each character, especially in relation to other characters
	- think exponents which are raised above the x-height (in an IDEAL world)
	- think subscript which are below the baseline
	- things like fractions have a horizontal line that separates the numerator and denominator
	- square roots are some characters with a character bounding them (radical sign)

- start with a simple equation, like 2+2=4

## characters:
- numbers (0-9)
- variables (a-z, A-Z)
- decimal point (.)
- sign (+, -)
- equal sign (=)
- greek letters/alphabet (α, β, γ, etc.)

## operators:
- addition (+)
- subtraction (-)
- multiplication (*, x, •) (two variables next to each other; e.g., AB or 2x is really A times B or 2 times x)
- division (/, ÷) (two variables separated by a horizontal line; e.g., A/B or 2/x is really A divided by B or 2 divided by x) (fractions)

- parentheses ((), [], {}) (a variable or expression inside; e.g., (x+2) is really x plus 2)

- exponents (^) (a variable raised above the x-height; e.g., x^2 is really x squared)
- square roots (√) (a radical sign with a variable inside; e.g., √x is really the square root of x)

- trigonometric functions (sin, cos, tan, etc.) (a function with a variable inside; e.g., sin(x) is really the sine of x)
- logarithms (log) (a function with a variable inside; e.g., log(x) is really the logarithm of x)

- vector notation (→) (a vector with a variable inside; e.g., →x is really the vector x)
- matrix notation ([, ]) (a matrix with variables inside; e.g., [x, y] is really the matrix x, y)


-- At this point only characters (numbers, math symbols) are present in the image we are working with
-- Segment by line, each object is assigned a line
-- Use position and character value to recognize equals, multi-digit numbers (i.e., if 1 and 2 are together with no object in between and their distance is reasonably small they can be assumed to form ‘12’)
-- Form math equation using LaTeX
-- Generate an output image! (https://github.com/goldsborough/latexpp)

Get contours of each numeric character
# of contours is a very rough tell:
1 contour: 1, 2, 3, 5, 7
2 contours: 0, 4, 6, 9
3 contours: 8
Roundness
Angle for adjustments
# of edges
# of corners
