#ifndef RENDERFORM_CHARACTER_H
#define RENDERFORM_CHARACTER_H

#include <iostream>
#include <opencv2/opencv.hpp>

namespace Renderform {
class Character {
public:
  const static int PADDING = 12;
  const static int CHARACTER_HEIGHT = 36;
  const static int CHARACTER_WIDTH = 36;

  static int getWidth();
  static int getHeight();

  Character();
  Character(cv::Mat component, cv::Rect bounding_box, cv::Point center);

  cv::Mat getComponent() const;
  cv::Rect getBoundingBox() const;
  cv::Point getCenter() const;
  cv::Rect getNormalizedBoundingBox() const;

  void setNormalizedBoundingBox(cv::Rect normalized_bounding_box);

  std::string getLabelKnown() const;
  std::string getLabelDetermined() const;
  std::string getLabelPredictedTesseract() const;
  std::string getLabelPredictedKnn() const;

  void setLabelKnown(std::string label);
  void setLabelDetermined(std::string label);
  void setLabelPredictedTesseract(std::string label);
  void setLabelPredictedKnn(std::string label);

  double getTesseractConfidence() const;
  double getKnnConfidence() const;

  void setTesseractConfidence(double confidence);
  void setKnnConfidence(double confidence);

  cv::Point getTopLeft() const;
  cv::Point getTopRight() const;
  cv::Point getBottomLeft() const;
  cv::Point getBottomRight() const;

  void calculateProperties();

private:
  cv::Mat component;
  cv::Rect bounding_box;
  cv::Rect normalized_bounding_box;
  cv::Point center;

  std::string label_known;
  std::string label_determined; // determined by object properties (like two
                                // rectangles are a '=')
  std::string label_predicted_tesseract;
  std::string label_predicted_knn;
  double tesseract_confidence;
  double knn_confidence;

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
