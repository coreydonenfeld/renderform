#ifndef RENDERFORM_FORMULA_H
#define RENDERFORM_FORMULA_H

#include <string>

namespace Renderform {

// @todo Possibly rename to "Expression"

class Formula {
public:
  Formula();

private:
  std::string m_formula_raw;
  // tree structure of the formula @todo
};

} // namespace Renderform

#endif // RENDERFORM_FORMULA_H
