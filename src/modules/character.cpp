#include "character.hpp"
#include "partitioner.hpp"

int Renderform::Character::getWidth() { return CHARACTER_WIDTH + 2 * PADDING; }
int Renderform::Character::getHeight() {
  return CHARACTER_HEIGHT + 2 * PADDING;
}

Renderform::Character::Character() {}

Renderform::Character::Character(cv::Mat component, cv::Rect bounding_box,
                                 cv::Point center)
    : component(component), bounding_box(bounding_box), center(center) {}

cv::Mat Renderform::Character::getComponent() const { return this->component; }
cv::Rect Renderform::Character::getBoundingBox() const {
  return this->bounding_box;
}
cv::Point Renderform::Character::getCenter() const { return this->center; }

cv::Point Renderform::Character::getTopLeft() const {
  return this->bounding_box.tl();
}

cv::Point Renderform::Character::getTopRight() const {
  return cv::Point(this->bounding_box.br().x, this->bounding_box.tl().y);
}

cv::Point Renderform::Character::getBottomRight() const {
  return this->bounding_box.br();
}

cv::Point Renderform::Character::getBottomLeft() const {
  return cv::Point(this->bounding_box.tl().x, this->bounding_box.br().y);
}

void Renderform::Character::calculateProperties() {
  // get contours of the connected component
  std::vector<std::vector<cv::Point>> character_contours;
  cv::findContours(this->component, character_contours, cv::RETR_TREE,
                   cv::CHAIN_APPROX_SIMPLE);

  if (character_contours.size() == 0) {
    std::cout << "No contours found for component ";
  }
}

std::string Renderform::Character::getLabelKnown() const {
  return this->label_known;
}
std::string Renderform::Character::getLabelDetermined() const {
  return this->label_determined;
}
std::string Renderform::Character::getLabelPredictedTesseract() const {
  return this->label_predicted_tesseract;
}
std::string Renderform::Character::getLabelPredictedKnn() const {
  return this->label_predicted_knn;
}

void Renderform::Character::setLabelKnown(std::string label) {
  this->label_known = label;
}
void Renderform::Character::setLabelDetermined(std::string label) {
  this->label_determined = label;
}
void Renderform::Character::setLabelPredictedTesseract(std::string label) {
  this->label_predicted_tesseract = label;
}
void Renderform::Character::setLabelPredictedKnn(std::string label) {
  this->label_predicted_knn = label;
}

double Renderform::Character::getTesseractConfidence() const {
  return this->tesseract_confidence;
}
double Renderform::Character::getKnnConfidence() const {
  return this->knn_confidence;
}

void Renderform::Character::setTesseractConfidence(double confidence) {
  this->tesseract_confidence = confidence;
}
void Renderform::Character::setKnnConfidence(double confidence) {
  this->knn_confidence = confidence;
}

cv::Rect Renderform::Character::getNormalizedBoundingBox() const {
  return this->normalized_bounding_box;
}

void Renderform::Character::setNormalizedBoundingBox(
    cv::Rect normalized_bounding_box) {
  this->normalized_bounding_box = normalized_bounding_box;
}
