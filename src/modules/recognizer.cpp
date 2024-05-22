#include "recognizer.hpp"

Renderform::Recognizer::Recognizer(std::string language)
    : tesseract_api(), language(language) {
  TesseractAPI().Init(NULL, this->language.c_str(), tesseract::OEM_LSTM_ONLY);
  TesseractAPI().SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  TesseractAPI().SetVariable("tessedit_char_whitelist",
                             "0123456789xyzXYZ()*/+-=<>JjoO");
}

Renderform::Recognizer::~Recognizer() {}

tesseract::TessBaseAPI &Renderform::Recognizer::TesseractAPI() {
  return this->tesseract_api;
}

std::string Renderform::Recognizer::recognize(Character &character) {
  TesseractAPI().SetImage(
      character.getComponent().data, character.getComponent().size().width,
      character.getComponent().size().height,
      character.getComponent().channels(), character.getComponent().step);

  std::string resultTesseract = std::string(TesseractAPI().GetUTF8Text());
  std::string resultKnn = Classifier::classifyKnn(character.getComponent());

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

  character.setLabelPredictedKnn(resultKnn);
  character.setLabelPredictedTesseract(resultTesseract);
  character.setKnnConfidence(1.0);
  character.setTesseractConfidence(TesseractAPI().MeanTextConf());

  return resultKnn;
}
