#include "knn.hpp"
#include <opencv2/highgui.hpp>

std::vector<cv::Mat> Renderform::extractDigits(const cv::Mat &img) {
  const int digitSize = 20;
  std::vector<cv::Mat> digits;
  digits.reserve(5000);

  for (int i = 0; i < img.rows; i += digitSize) {
    for (int j = 0; j < img.cols; j += digitSize) {
      cv::Rect roi = cv::Rect(j, i, digitSize, digitSize);
      cv::Mat digitImg = img(roi);
      digits.push_back(digitImg);
    }
  }

  return digits;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Renderform::deskewDigits(std::vector<cv::Mat> &digits) {
  for (auto &digit : digits) {
    cv::Moments m = cv::moments(digit);

    if (abs(m.mu02) < 1e-2)
      return;

    float skew = m.mu11 / m.mu02;
    cv::Mat warpMat =
        (cv::Mat_<float>(2, 3) << 1, skew, -0.5 * digit.rows * skew, 0, 1, 0);
    cv::warpAffine(digit, digit, warpMat, digit.size(),
                   cv::WARP_INVERSE_MAP | cv::INTER_LINEAR);
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cv::Mat Renderform::extractFeatures(const std::vector<cv::Mat> &digits) {
  cv::HOGDescriptor hog(
      cv::Size(20, 20), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9, 1,
      -1, cv::HOGDescriptor::HistogramNormType::L2Hys, 0.2, 0, 64, 0);

  const int featueSize = hog.getDescriptorSize();

  cv::Mat features(digits.size(), featueSize, CV_32FC1);

  for (int i = 0; i < digits.size(); i++) {
    const auto &digitImg = digits[i];
    std::vector<float> descriptor;
    hog.compute(digitImg, descriptor);
    float *featuresPtr = features.ptr<float>(i);
    memcpy(featuresPtr, descriptor.data(), featueSize * sizeof(float));
  }

  return features;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cv::Mat Renderform::loadLabels() {
  cv::Mat labels(5000, 1, CV_32SC1);
  int32_t *labelsPtr = labels.ptr<int32_t>(0);

  for (int i = 0; i < 5000; i++)
    labelsPtr[i] = i / 500;

  return labels;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cv::Ptr<cv::ml::TrainData>
Renderform::createTrainData(const std::string &imgPath) {
  cv::Mat img = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);

  auto digitImages = extractDigits(img);
  deskewDigits(digitImages);
  auto features = extractFeatures(digitImages);

  auto labels = loadLabels();

  cv::Ptr<cv::ml::TrainData> data =
      cv::ml::TrainData::create(features, cv::ml::ROW_SAMPLE, labels);

  return data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Renderform::trainKnn(const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto k_nearest = cv::ml::KNearest::create();

  k_nearest->setDefaultK(7);
  k_nearest->setIsClassifier(true);

  k_nearest->train(dataset);

  k_nearest->save("KNN.xml");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float Renderform::testKnn(const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto k_nearest = cv::ml::KNearest::load("KNN.xml");

  std::vector<int32_t> predictions;
  auto error = k_nearest->calcError(dataset, true, predictions);

  return error;
}

int Renderform::classifyKnn(const cv::Mat &img) {
  // input cv::Mat image, output predicted label
  // 0-9 for digits, 10 for non-digit
  // -1 for error

  cv::Ptr<cv::ml::KNearest> knn = cv::ml::KNearest::load("KNN.xml");
  cv::HOGDescriptor hog(
      cv::Size(20, 20), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9, 1,
      -1, cv::HOGDescriptor::HistogramNormType::L2Hys, 0.2, 0, 64, 0);

  cv::Mat digitImg = img;
  cv::resize(digitImg, digitImg, cv::Size(20, 20));
  std::vector<float> descriptor;
  hog.compute(digitImg, descriptor);

  cv::Mat features(1, descriptor.size(), CV_32FC1);
  float *featuresPtr = features.ptr<float>(0);
  memcpy(featuresPtr, descriptor.data(), descriptor.size() * sizeof(float));

  cv::Mat response, neighborIndices, distances;
  auto nearest_response =
      knn->findNearest(features, 1, response, neighborIndices, distances);

  if (distances.empty() || response.empty()) {
    std::cerr << "Error during KNN prediction!" << std::endl;
    return -1; // Error during prediction
  }

  float distance = distances.at<float>(0, 0);
  float predictedLabel = response.at<float>(0, 0);
  // std::cout << "nearest response: " << nearest_response
  //           << ", distance: " << distance << std::endl;

  // Optionally, use the distance to make decisions
  // For example, if the distance is too high, classify as non-digit (10)
  // const double INCORRECT_DISTANCE_THRESHOLD = 10.0;
  // if (distance > INCORRECT_DISTANCE_THRESHOLD) {
  //   return -1; // Non-digit
  // }

  return static_cast<int>(predictedLabel);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Renderform::trainSVM(const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto svm = cv::ml::SVM::create();

  svm->setKernel(
      cv::ml::SVM::LINEAR); // cv::ml::SVM::RBF, cv::ml::SVM::SIGMOID,
                            // cv::ml::SVM::POLY
  svm->setType(cv::ml::SVM::C_SVC);

  svm->trainAuto(dataset);

  svm->save("SVM.xml");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float Renderform::testSVM(const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto svm = cv::ml::SVM::load("SVM.xml");

  std::vector<int32_t> predictions;
  auto error = svm->calcError(dataset, true, predictions);

  return error;
}

int Renderform::classifySVM(const cv::Mat &img) {
  auto svm = cv::ml::SVM::load("SVM.xml");
  cv::HOGDescriptor hog(
      cv::Size(20, 20), cv::Size(8, 8), cv::Size(4, 4), cv::Size(8, 8), 9, 1,
      -1, cv::HOGDescriptor::HistogramNormType::L2Hys, 0.2, 0, 64, 1);

  cv::Mat digitImg = img;
  cv::resize(digitImg, digitImg, cv::Size(20, 20));
  std::vector<float> descriptor;
  hog.compute(digitImg, descriptor);

  cv::Mat features(1, descriptor.size(), CV_32FC1);
  float *featuresPtr = features.ptr<float>(0);
  memcpy(featuresPtr, descriptor.data(), descriptor.size() * sizeof(float));

  cv::Mat response;
  svm->predict(features, response);

  return response.at<float>(0, 0);
}
