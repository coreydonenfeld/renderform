#ifndef RENDERFORM_H
#define RENDERFORM_H

#include "formula.hpp"

namespace Renderform {};

#endif // RENDERFORM_H

/*

want this class to be used as API by end user like this:
Renderform::parseFormula("/path/to/img.png"); // return a Formula object
rf::Formula f = Renderform::parseFormula("/path/to/img.png");
f.checkAgainst("x^2 + y^2 = 1"); // returns true or false
f.getFormula(); // returns the formula string
f.toLatex(); // returns the formula in latex format
Renderform::renderLatex(f.toLatex()); // renders the latex formula

*/
