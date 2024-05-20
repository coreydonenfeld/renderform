#ifndef RENDERFORM_CHARACTER_H
#define RENDERFORM_CHARACTER_H

#include <iostream>
#include <opencv2/opencv.hpp>

namespace Renderform {
class Character {
public:
  Character(cv::Mat component, cv::Rect bounding_box, cv::Point center);

  cv::Mat getComponent() const;
  cv::Rect getBoundingBox() const;
  cv::Point getCenter() const;

  cv::Point getTopLeft() const;
  cv::Point getTopRight() const;
  cv::Point getBottomLeft() const;
  cv::Point getBottomRight() const;

  void calculateProperties();

private:
  cv::Mat component;
  cv::Rect bounding_box;
  cv::Point center;

  double area;
  double aspect_ratio;
  double compactness;
  double eccentricity;
  double extent;
  double orientation;
  double solidity;
  double perimeter;
  double circularity;
  double roundness;
  double elongation;
  double convexity;
};
} // namespace Renderform

#endif // RENDERFORM_CHARACTER_H
