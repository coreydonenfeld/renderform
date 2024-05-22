#ifndef RENDERFORM_CLASSIFIER_KNN_HPP
#define RENDERFORM_CLASSIFIER_KNN_HPP

/*
OpenCV ML Module / kNN implementation / Derived from
https://github.com/zanazakaryaie/digit_classifiers
*/

#include "symbols.hpp"
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include <string>
#include <utility>

namespace Renderform::Classifier {

const int KNN_K = 9;
const int IMAGE_SIZE = 32;

cv::HOGDescriptor getHogDescriptor();

cv::Ptr<cv::ml::TrainData> extractAndTrainClassifierImages();

std::vector<cv::Mat> extractImagesFromGroup(int symbol, int maximum = -1);

std::vector<cv::Mat> extractDigits(const cv::Mat &img);

void deskewDigits(std::vector<cv::Mat> &digits);

cv::Mat extractFeatures(const std::vector<cv::Mat> &digits);

cv::Mat loadLabels();

cv::Ptr<cv::ml::TrainData> createTrainData(const std::string &imgPath);

void trainKnn(const cv::Ptr<cv::ml::TrainData> &dataset);
float testKnn(const cv::Ptr<cv::ml::TrainData> &dataset);
std::string classifyKnn(const cv::Mat &img);

// SVM (@todo Move to svm.hpp file)
void trainSVM(const cv::Ptr<cv::ml::TrainData> &dataset);
float testSVM(const cv::Ptr<cv::ml::TrainData> &dataset);
std::string classifySVM(const cv::Mat &img);

} // namespace Renderform::Classifier

#endif // RENDERFORM_CLASSIFIER_KNN_HPP
