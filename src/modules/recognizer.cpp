#include "recognizer.hpp"
#include "tesseract/publictypes.h"

Renderform::Recognizer::Recognizer(std::string language)
    : tesseract_api(), language(language) {
  TesseractAPI().Init(NULL, this->language.c_str(), tesseract::OEM_LSTM_ONLY);
  TesseractAPI().SetPageSegMode(tesseract::PSM_RAW_LINE);
  // TesseractAPI().SetVariable("edges_min_nonhole", "80");

  // tesseract::TessBaseAPI tess;
  // tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
  // tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  // // tess.SetVariable("classify_bln_numeric_mode", "1");

  // // whitelist digits and operators
  // tess.SetVariable("tessedit_char_whitelist", "0123456789=+-*/xyzXYZ");
  // // tess.SetVariable("tessedit_char_whitelist", "+-=-~—x*•");
  // // tess.SetVariable("user_defined_dpi", "300");
  // // tess.SetVariable("matcher_good_threshold", "0");
  // // tess.SetVariable("classify_enable_learning", "1");
  // // tess.SetVariable("classify_enable_adaptive_matcher", "1");
  // // tess.SetVariable("tessedit_char_blacklist", "`~@");
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

  // get hOCR output
  char *hocr = TesseractAPI().GetHOCRText(0);

  std::cout << "hOCR: " << hocr << std::endl;

  /*
  If response is:
  "\" -> "1"
  "xX" -> "x"
  */

  return std::string(TesseractAPI().GetUTF8Text());
}
