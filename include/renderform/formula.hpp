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
@see https://craftinginterpreters.com/parsing-expressions.html

2 + 2 = 4

    =
   / \
  +   4
  / \
  2   2

*/

enum class FormType { EXPRESSION, EQUATION, INEQUALITY, FUNCTION };

class Formula {
public:
  Formula();
  Formula(std::string formula, FormType type = FormType::EXPRESSION);

  static bool areBracketsBalanced(const std::string &input);
  static std::string concealInnerGroup(const std::string &input);

  // bool isValid();
  void simplify();

  void print();

private:
  FormType formula_type; // not really needed! Or maybe it is?
  std::string formula_raw;
  std::string formula_tex;
  Node *root = nullptr;

  Node *parseString(const std::string &formula, Node *iterator, int depth = 0);
  Node *parseExpression(const std::string &expression, Node *iterator,
                        int depth = 0);
  Node *parseEquation(const std::string &equation, Node *iterator,
                      int depth = 0);

  Node *parseLiteral(const std::string &literal, Node *iterator, int depth = 0);
  Node *parseGroup(const std::string &group, Node *iterator, int depth = 0);
  Node *parseUnary(const std::string &unary, Node *iterator, int depth = 0);
  Node *parseBinary(const std::string &binary, Node *iterator, int depth = 0);
};

} // namespace Renderform

#endif // RENDERFORM_FORMULA_H
