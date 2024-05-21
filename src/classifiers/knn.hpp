#ifndef RENDERFORM_CLASSIFIER_KNN_HPP
#define RENDERFORM_CLASSIFIER_KNN_HPP

/*
OpenCV ML Module / kNN implementation / Derived from
https://github.com/zanazakaryaie/digit_classifiers
*/

#include <iostream>
#include <opencv2/opencv.hpp>

namespace Renderform {

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector<cv::Mat> extractDigits(const cv::Mat &img);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void deskewDigits(std::vector<cv::Mat> &digits);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cv::Mat extractFeatures(const std::vector<cv::Mat> &digits);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cv::Mat loadLabels();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cv::Ptr<cv::ml::TrainData> createTrainData(const std::string &imgPath);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void trainKnn(const cv::Ptr<cv::ml::TrainData> &dataset);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float testKnn(const cv::Ptr<cv::ml::TrainData> &dataset);

// input cv::Mat image, output predicted label
// 0-9 for digits, 10 for non-digit
// -1 for error
int classifyKnn(const cv::Mat &img);

void trainSVM(const cv::Ptr<cv::ml::TrainData> &dataset);


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float testSVM(const cv::Ptr<cv::ml::TrainData> &dataset);

int classifySVM(const cv::Mat &img);

} // namespace Renderform

#endif // RENDERFORM_CLASSIFIER_KNN_HPP
