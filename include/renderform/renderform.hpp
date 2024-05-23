#ifndef RENDERFORM_H
#define RENDERFORM_H

#include "formula.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

namespace Renderform {

Formula parseImage(const std::string &image_path);
Formula parseText(const std::string &text);

}; // namespace Renderform

#endif // RENDERFORM_H
