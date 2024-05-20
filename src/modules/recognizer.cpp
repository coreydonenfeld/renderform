#include "recognizer.hpp"
#include "tesseract/publictypes.h"

Renderform::Recognizer::Recognizer(std::string language)
    : tesseract_api(), language(language) {
  TesseractAPI().Init(NULL, this->language.c_str(), tesseract::OEM_LSTM_ONLY);
}

Renderform::Recognizer::~Recognizer() {}

tesseract::TessBaseAPI &Renderform::Recognizer::TesseractAPI() {
  return this->tesseract_api;
}

std::string Renderform::Recognizer::recognize(const Character &character) {
  TesseractAPI().SetImage(
      character.getComponent().data, character.getComponent().size().width,
      character.getComponent().size().height,
      character.getComponent().channels(), character.getComponent().step);

  std::string result = std::string(TesseractAPI().GetUTF8Text());

  // Remove newlines
  result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

  return result;
}
