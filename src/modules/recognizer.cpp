#include "recognizer.hpp"
#include <opencv2/imgcodecs.hpp>

Renderform::Recognizer::Recognizer(std::string language)
    : tesseract_api(), language(language) {
  TesseractAPI().Init(NULL, this->language.c_str(), tesseract::OEM_LSTM_ONLY);
  TesseractAPI().SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  TesseractAPI().SetVariable("tessedit_char_whitelist",
                             "0123456789xyzXYZ()*/+-=<>JjoO");

  cv::Mat image = cv::imread("../data/0-9-digits.png", cv::IMREAD_COLOR);

  auto dataSet = createTrainData("../data/0-9-digits.png");
  dataSet->setTrainTestSplitRatio(0.8, true);
  trainKnn(dataSet);
  // trainSVM(dataSet);
  float knnError = testKnn(dataSet);
  std::cout << "KNN error: " << knnError << std::endl;
  // float svmError = testSVM(dataSet);
  // std::cout << "SVM error: " << svmError << std::endl;
}

Renderform::Recognizer::~Recognizer() {}

tesseract::TessBaseAPI &Renderform::Recognizer::TesseractAPI() {
  return this->tesseract_api;
}

// @maybe see https://github.com/hdyoon/MathsymbolRecog?tab=readme-ov-file
//
// https://epub.jku.at/obvulihs/download/pdf/3866590?originalFilename=true
//
// possibly look to train: https://github.com/wblachowski/bhmsds

std::string Renderform::Recognizer::recognize(Character &character) {
  cv::Mat classify_image = character.getComponent();
  // cv::resize(classify_image, classify_image, cv::Size(20, 20));
  // cv::imshow("test_img", classify_image);
  // cv::waitKey(0);
  // std::cout << "Knn classify: " << classifyKnn(classify_image) << std::endl;
  // std::cout << classifySVM(classify_image) << std::endl;
  // std::cout << "KNN error: " << knnError << std::endl;

  TesseractAPI().SetImage(
      character.getComponent().data, character.getComponent().size().width,
      character.getComponent().size().height,
      character.getComponent().channels(), character.getComponent().step);

  std::string resultTesseract = std::string(TesseractAPI().GetUTF8Text());
  std::string resultKnn = std::to_string(classifyKnn(classify_image));

  // Remove newlines
  resultTesseract.erase(
      std::remove(resultTesseract.begin(), resultTesseract.end(), '\n'),
      resultTesseract.end());
  resultKnn.erase(std::remove(resultKnn.begin(), resultKnn.end(), '\r'),
                  resultKnn.end());

  // If tesseract has more than one character, take the first one
  if (resultTesseract.size() > 1) {
    resultTesseract = resultTesseract.substr(0, 1);
  }

  // @todo Really small 0 or o or O should be treated as * (multiplication)

  character.setLabelPredictedKnn(resultKnn);
  character.setLabelPredictedTesseract(resultTesseract);
  character.setKnnConfidence(1.0);
  character.setTesseractConfidence(TesseractAPI().MeanTextConf());

  return resultKnn;
}
