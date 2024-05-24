# Renderform
Renderform is a handwritten mathematical formula recognition system that processes images of handwritten text and parses them into recognized mathematical formulas, which can be rendered in LaTeX. The system is designed to be user-friendly and accessible, providing a simple C++ API and CLI for users to interact with. Renderform is built using C++ and OpenCV, leveraging the k-Nearest Neighbors (k-NN) and Support Vector Machine (SVM) classifiers for character recognition. The system also uses Tesseract OCR for additional character recognition capabilities. Renderform started as my final computer vision project and I am interesed in continuing to develop it beyond the scope of the class.

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
- In progress: Generate an image of the LaTeX-rendered equation (not yet implemented); using https://latex2image.joeraut.com/ for now to visualize the LaTeX code

## Demo
Above each character is Tesseract's recognize character followed by a space and then the k-NN recognized character. The standard output shows each character's prediction and the end shows the formula for each line.

#### 1+1=2
![Demo 1: 1+1=2](/data/demo/1+1=2.png)

#### 5*2=y
![Demo 2: 5*2=y](/data/demo/5*2=y.png)

#### 7+9=3x
![Demo 3: 7+9=3x](/data/demo/7+9=3x.png)

#### s(p) - untrained symbols (s, p)
![Demo 4: s(p)](/data/demo/s(p).png)

#### Parse Tree to LaTeX
![Demo 5: Parse Tree to LaTeX](/data/demo/parse-tree.png)

## Usage
### Dependencies
- OpenCV 4.9.0 (https://github.com/opencv/opencv)
- CMake 3.24 (https://cmake.org/)
- C++17 (https://en.cppreference.com/w/cpp/17)
- Tesseract 5.3.4 (https://github.com/tesseract-ocr/tesseract)
- Leptonica 1.84.1 (bundled with Tesseract)
- LaTeX (optional, for rendering LaTeX code)
### Data
KNN.xml (pre-classified) - https://drive.google.com/file/d/1vUeZsTvAzKuYZ-wa6b5WGKZYK3MQXolJ/view?usp=sharing

Character data - https://drive.google.com/file/d/1nvah1mQSnnk0nJhoNxKLfkCNt2icgglw/view?usp=sharing
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
  // Get LaTeX formula
  std::string latex = output.getLaTeX();
  // Print LaTeX formula
  std::cout << latex << std::endl;
  return 0;
}
```
### CLI
Parse and print formula from image:
```bash
./renderform <path_to_image>
```
General usage:
```bash
./renderform
```
Enter mode: "image", "manual", "train"
```bash
> image
> ../data/demo/1+1=2.png
> [PROGRAM_OUTPUT]...
> ../data/demo/5*2=y.png
```
```bash
> manual
> 1+1=2
```
```bash
> train
```

## A Note on Character Recognition
The recognition uses a mix of OpenCV machine learning models and Tesseract to attempt to recognize characters. It is fairly slow and unreliable, but it is a good starting point! The goal is to improve the recognition system in the future.

### k-NN Classifier (OpenCV Implementation)
https://docs.opencv.org/4.9.0/d8/d4b/tutorial_py_knn_opencv.html

k-NN is a great approach for individual character recognition but gets very slow, expensive, and is not very scalable. It proved to work nicely for most expressions but failed occasionally, yielding jumbled incorrect results. The k-NN classifier is a great starting point for this project but is not the best solution for the final product. Perhaps in the future, it would be ideal to implement a more sophisticated model, such as a Convolutional Neural Network (CNN), to improve the performance and accuracy of the character recognition system.

### SVM Classifier (OpenCV Implementation)
https://docs.opencv.org/4.x/d1/d73/tutorial_introduction_to_svm.html

SVM is a more sophisticated model that could potentially improve the performance of the character recognition system. It is more complex and requires more data and training time but could yield better results. I implemented the SVM classifier but could not tune the parameters effectively to improve the performance. The SVM classifier is a great option for this project but requires more time and resources to optimize.

### Tesseract OCR
https://tesseract-ocr.github.io/

Tesseract OCR is a great tool for character recognition but *not* for handwritten text. It particularly fails when the input contains math symbols and expressions. The pre-trained model for English is extremely impressive yet flawed for this project's specific purposes of parsing handwritten mathematical formulas. Tesseract seemed like a great option prior to beginning the project. Unfortunately, it yielded subpar, haphazard results. A stretch goal, beyond the scope of this project in its current state, would be to train Tesseract with custom data, potentially from the MNIST database, to improve its performance.

## Inspiration & References
- https://blog.ayoungprogrammer.com/2013/01/equation-ocr-part-2-training-characters.html
- https://github.com/floneum/floneum/blob/main/models/kalosm-ocr/examples/ocr.rs
- https://github.com/stevenobadja/math_object_detection?tab=readme-ov-file
- https://arxiv.org/pdf/1003.5898
- https://groups.google.com/g/tesseract-ocr/c/lvr_xaSuSe0
- https://www.youtube.com/watch?v=a5oeEhTf6_M
- https://www.kaggle.com/datasets/xainano/handwrittenmathsymbols/data
- https://github.com/opencv/opencv/blob/master/samples/dnn/text_detection.cpp
- https://latex2image.joeraut.com/
- https://github.com/zanazakaryaie/digit_classifiers
