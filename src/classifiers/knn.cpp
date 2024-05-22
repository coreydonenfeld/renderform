#include "knn.hpp"
#include "symbols.hpp"
#include <random>

cv::HOGDescriptor Renderform::Classifier::getHogDescriptor() {
  return cv::HOGDescriptor(cv::Size(IMAGE_SIZE, IMAGE_SIZE), cv::Size(8, 8),
                           cv::Size(4, 4), cv::Size(4, 4), 9, 1, -1,
                           cv::HOGDescriptor::HistogramNormType::L2Hys, 0.2, 0,
                           64, 0);
}

// Data set obtained from:
// https://www.kaggle.com/datasets/xainano/handwrittenmathsymbols/data
cv::Ptr<cv::ml::TrainData>
Renderform::Classifier::extractAndTrainClassifierImages() {
  int num_symbols = 10; // 10 digits
  std::vector<int> extra_variables = {SYMBOL_CHAR_X, SYMBOL_CHAR_Y,
                                      SYMBOL_CHAR_Z};
  std::vector<int> extra_symbols = {SYMBOL_OP_PLUS,     SYMBOL_OP_MINUS,
                                    SYMBOL_OP_EQUALS,   SYMBOL_OP_TIMES,
                                    SYMBOL_OP_DIVIDE,   SYMBOL_GROUP_LPAREN,
                                    SYMBOL_GROUP_RPAREN};
  num_symbols += extra_symbols.size();

  std::cout << "Loading dataset..." << std::endl;

  std::vector<std::pair<int, std::vector<cv::Mat>>> dataset;
  dataset.reserve(num_symbols); // 10 different symbols

  int total_size = 0;
  for (int i = 0; i < 10; i++) {
    auto images = extractImagesFromGroup(i, 1500);
    dataset.push_back(std::make_pair(i, images));
    total_size += images.size();
  }

  for (const auto &symbol : extra_variables) {
    auto images = extractImagesFromGroup(symbol, 1500);
    dataset.push_back(std::make_pair(symbol, images));
    total_size += images.size();
  }

  for (const auto &symbol : extra_symbols) {
    auto images = extractImagesFromGroup(symbol, 1500);
    dataset.push_back(std::make_pair(symbol, images));
    total_size += images.size();
  }

  cv::HOGDescriptor hog = getHogDescriptor();
  const int featureSize = hog.getDescriptorSize();

  std::cout << "Extracting features..." << std::endl;
  std::cout << "mat total size: " << total_size << std::endl;
  std::cout << "Feature size: " << featureSize << std::endl;

  std::vector<cv::Mat> featuresGroups;

  cv::Mat features(total_size, featureSize, CV_32FC1);
  cv::Mat labels(total_size, 1, CV_32SC1);
  int32_t *labelsPtr = labels.ptr<int32_t>(0);

  int counter = 0;
  for (const auto &symbol : dataset) {
    std::cout << "Symbol " << symbol.first << std::endl;
    for (const auto &img : symbol.second) {
      std::vector<float> descriptor;
      hog.compute(img, descriptor);
      float *featuresPtr = features.ptr<float>(counter);
      memcpy(featuresPtr, descriptor.data(), featureSize * sizeof(float));
      labelsPtr[counter] = symbol.first;
      counter++;
    }
    std::cout << "Symbol " << symbol.first << " features computed" << std::endl;
    std::cout << std::endl;
  }

  std::cout << "Training classifier..." << std::endl;
  std::cout << "Number of samples: " << features.rows << std::endl;
  std::cout << "Number of features: " << features.cols << std::endl;
  std::cout << "Number of labels: " << labels.rows << std::endl;

  cv::Ptr<cv::ml::TrainData> data =
      cv::ml::TrainData::create(features, cv::ml::ROW_SAMPLE, labels);

  std::cout << "Data ready" << std::endl;

  return data;
}

std::vector<cv::Mat>
Renderform::Classifier::extractImagesFromGroup(int symbol, int maximum) {
  std::pair<int, std::string> groupAndSymbol =
      Renderform::getGroupAndSymbolFromNumber(symbol);

  std::string group_folder_name;
  switch (groupAndSymbol.first) {
  case 0:
    group_folder_name = "0_DIGIT";
    break;
  case 1:
    group_folder_name = "1_CHAR";
    break;
  case 2:
    group_folder_name = "2_OP";
    break;
  case 3:
    group_folder_name = "3_GROUP";
    break;
  }

  cv::String path = "../Source/data/training/" + group_folder_name + "/" +
                    groupAndSymbol.second + "/*.jpg";

  std::cout << "Loading symbol \"" << groupAndSymbol.second << "\" (" << symbol
            << ") from group \"" << group_folder_name << "\"" << std::endl;

  // cv::String path("../Source/data/training/0_DIGIT/0/*.jpg");

  std::vector<cv::String> fn;
  std::vector<cv::Mat> data;
  cv::glob(path, fn, true); // recurse

  // randomize order of fn
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(fn.begin(), fn.end(), g);

  for (size_t k = 0; k < fn.size(); ++k) {
    cv::Mat im = cv::imread(fn[k], cv::IMREAD_GRAYSCALE);
    if (im.empty()) {
      std::cout << "Warning: Image " << fn[k] << " is empty" << std::endl;
      continue;
    }
    if (maximum != -1 && data.size() >= maximum) {
      std::cout << "Maximum number of images (" << maximum << ") reached"
                << std::endl;
      break;
    }
    cv::resize(im, im, cv::Size(IMAGE_SIZE, IMAGE_SIZE));
    cv::threshold(im, im, 128, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::erode(im, im, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
    data.push_back(im);
  }
  Renderform::Classifier::deskewDigits(data);
  std::cout << "Number of images in dataset: " << data.size() << std::endl;
  return data;
}

std::vector<cv::Mat> Renderform::Classifier::extractDigits(const cv::Mat &img) {
  const int digitSize = 20;
  std::vector<cv::Mat> digits;
  digits.reserve(5000);

  for (int i = 0; i < img.rows; i += digitSize) {
    for (int j = 0; j < img.cols; j += digitSize) {
      cv::Rect roi = cv::Rect(j, i, digitSize, digitSize);
      cv::Mat digitImg = img(roi);
      cv::resize(digitImg, digitImg, cv::Size(IMAGE_SIZE, IMAGE_SIZE));
      digits.push_back(digitImg);
    }
  }

  return digits;
}

void Renderform::Classifier::deskewDigits(std::vector<cv::Mat> &digits) {
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

cv::Mat
Renderform::Classifier::extractFeatures(const std::vector<cv::Mat> &digits) {
  cv::HOGDescriptor hog(cv::Size(IMAGE_SIZE, IMAGE_SIZE), cv::Size(8, 8),
                        cv::Size(4, 4), cv::Size(4, 4), 9, 1, -1,
                        cv::HOGDescriptor::HistogramNormType::L2Hys, 0.2, 0, 64,
                        0);

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

cv::Mat Renderform::Classifier::loadLabels() {
  cv::Mat labels(5000, 1, CV_32SC1);
  int32_t *labelsPtr = labels.ptr<int32_t>(0);

  for (int i = 0; i < 5000; i++)
    labelsPtr[i] = i / 500;

  return labels;
}

// @deprecated Use extractAndTrainClassifierImages instead
cv::Ptr<cv::ml::TrainData>
Renderform::Classifier::createTrainData(const std::string &imgPath) {
  cv::Mat img = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);

  auto digitImages = extractDigits(img);
  deskewDigits(digitImages);
  auto features = extractFeatures(digitImages);

  auto labels = loadLabels();

  cv::Ptr<cv::ml::TrainData> data =
      cv::ml::TrainData::create(features, cv::ml::ROW_SAMPLE, labels);

  return data;
}

void Renderform::Classifier::trainKnn(
    const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto k_nearest = cv::ml::KNearest::create();

  k_nearest->setDefaultK(KNN_K);
  k_nearest->setIsClassifier(true);

  k_nearest->train(dataset);

  std::cout << "Saving KNN model to KNN.xml" << std::endl;

  k_nearest->save("KNN.xml");
}

float Renderform::Classifier::testKnn(
    const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto k_nearest = cv::ml::KNearest::load("KNN.xml");

  std::vector<int32_t> predictions;
  auto error = k_nearest->calcError(dataset, true, predictions);

  return error;
}

std::string Renderform::Classifier::classifyKnn(const cv::Mat &img) {
  // input cv::Mat image, output predicted label as string
  // 0-9 for digits, >10 maps to SYMBOLS specified in symbols.hpp

  cv::Ptr<cv::ml::KNearest> knn = cv::ml::KNearest::load("KNN.xml");
  cv::HOGDescriptor hog = getHogDescriptor();

  cv::Mat digitImg = img;
  cv::resize(digitImg, digitImg, cv::Size(IMAGE_SIZE, IMAGE_SIZE));
  std::vector<float> descriptor;
  hog.compute(digitImg, descriptor);

  cv::Mat features(1, descriptor.size(), CV_32FC1);
  float *featuresPtr = features.ptr<float>(0);
  memcpy(featuresPtr, descriptor.data(), descriptor.size() * sizeof(float));

  cv::Mat response, neighborIndices, distances;
  auto nearest_response =
      knn->findNearest(features, 3, response, neighborIndices, distances);

  if (distances.empty() || response.empty()) {
    std::cerr << "Error during KNN prediction!" << std::endl;
    return "-1"; // Error during prediction
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

  std::pair<int, std::string> result =
      getGroupAndSymbolFromNumber(static_cast<int>(predictedLabel));

  return result.second;
}
// SVM
void Renderform::Classifier::trainSVM(
    const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto svm = cv::ml::SVM::create();
  svm->setKernel(cv::ml::SVM::RBF); // You can change to other kernels like
                                    // RBF, POLY, etc.
  svm->setType(cv::ml::SVM::C_SVC);
  svm->setC(2.67);
  svm->setGamma(5.383); // Only needed for RBF kernel

  std::cout << "Training SVM..." << std::endl;
  svm->train(dataset);

  std::cout << "Saving SVM model to SVM.xml" << std::endl;
  svm->save("SVM.xml");
}

float Renderform::Classifier::testSVM(
    const cv::Ptr<cv::ml::TrainData> &dataset) {
  auto svm = cv::ml::SVM::load("SVM.xml");

  cv::Mat predictions;
  svm->predict(dataset->getSamples(), predictions);

  cv::Mat responses = dataset->getResponses();
  int correctPredictions = 0;
  for (int i = 0; i < predictions.rows; ++i) {
    if (predictions.at<float>(i, 0) == responses.at<int>(i, 0)) {
      correctPredictions++;
    }
  }

  float accuracy = (float)correctPredictions / predictions.rows;
  return (1.0f - accuracy) * 100.0f; // Returning error percentage
}

std::string Renderform::Classifier::classifySVM(const cv::Mat &img) {
  cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("SVM.xml");
  cv::HOGDescriptor hog = getHogDescriptor();

  cv::Mat digitImg = img;
  cv::resize(digitImg, digitImg, cv::Size(IMAGE_SIZE, IMAGE_SIZE));
  std::vector<float> descriptor;
  hog.compute(digitImg, descriptor);

  cv::Mat features(1, descriptor.size(), CV_32FC1);
  float *featuresPtr = features.ptr<float>(0);
  memcpy(featuresPtr, descriptor.data(), descriptor.size() * sizeof(float));

  float predictedLabel = svm->predict(features);

  std::pair<int, std::string> result =
      getGroupAndSymbolFromNumber(static_cast<int>(predictedLabel));
  return result.second;
}
