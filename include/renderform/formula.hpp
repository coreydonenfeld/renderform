#ifndef RENDERFORM_FORMULA_H
#define RENDERFORM_FORMULA_H

#include "formtree.hpp"
#include <iostream>
#include <map>
#include <regex>
#include <stack>
#include <string>
#include <vector>

namespace Renderform {

/*
Represent a formula in a tree structure. The formula can be an expression,
equation, inequality, or function.

2 + 2 = 4
Turned into a parse tree:
    =
   / \
  +   4
 / \
2   2

Each node is either an operator or a value. A value can be a number or a
variable.

@see https://craftinginterpreters.com/parsing-expressions.html
*/

enum class FormType { EXPRESSION, EQUATION, INEQUALITY, FUNCTION };

class Formula {
public:
  Formula();
  Formula(std::string formula, FormType type = FormType::EXPRESSION);

  static bool areBracketsBalanced(const std::string &input);
  static std::string concealInnerGroup(const std::string &input);

  void print();

  std::string getLaTeX();

private:
  FormType formula_type; // not really needed! Or maybe it is?
  std::string formula_raw;
  std::string formula_tex;
  Node *root = nullptr;

  Node *parseString(const std::string &formula, Node *iterator, int depth = 0);
  Node *parseExpression(const std::string &expression, Node *iterator,
                        int depth = 0, bool use_parentheses = false);
  Node *parseEquation(const std::string &equation, Node *iterator,
                      int depth = 0);

  std::string getLaTeX(Node *node);
};

} // namespace Renderform

#endif // RENDERFORM_FORMULA_H
