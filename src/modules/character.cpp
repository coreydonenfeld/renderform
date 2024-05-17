#include "character.hpp"
#include "partitioner.hpp"

Renderform::Character::Character(cv::Mat component, cv::Rect bounding_box,
                                 cv::Point center)
    : component(component), bounding_box(bounding_box), center(center) {}

cv::Mat Renderform::Character::getComponent() const { return this->component; }
cv::Rect Renderform::Character::getBoundingBox() const {
  return this->bounding_box;
}
cv::Point Renderform::Character::getCenter() const { return this->center; }
