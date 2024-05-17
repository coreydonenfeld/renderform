#ifndef RENDERFORM_FORMULA_H
#define RENDERFORM_FORMULA_H

#include <iostream>
#include <map>
#include <regex>
#include <stack>
#include <string>
#include <variant>
#include <vector>

namespace Renderform {

// @todo Possibly rename to "Expression"
//

/*
@see https://craftinginterpreters.com/parsing-expressions.html

2 + 2 = 4

    =
   / \
  +   4
  / \
  2   2

*/

enum class FormType { EXPRESSION, EQUATION, INEQUALITY, FUNCTION, MATRIX };
enum class NodeType { OPERATOR, NUMBER, VARIABLE };
enum class Operator {
  EQUAL,
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  POWER,
  ROOT,
  // BASE,
  // ROOT,
  // LOG,
  // LN,
  // SIN,
  // COS,
  // TAN,
  // FACTORIAL,
};

struct NodeOperator {
  Operator op;
  // allow std::cout to print the value
  friend std::ostream &operator<<(std::ostream &os, const NodeOperator &op) {
    switch (op.op) {
    case Operator::EQUAL:
      os << "=";
      break;
    case Operator::ADD:
      os << "+";
      break;
    case Operator::SUBTRACT:
      os << "-";
      break;
    case Operator::MULTIPLY:
      os << "*";
      break;
    case Operator::DIVIDE:
      os << "/";
      break;
    case Operator::POWER:
      os << "^";
      break;
    case Operator::ROOT:
      os << "√";
      break;
    }
    return os;
  }

  NodeOperator() : op() {}
  NodeOperator(char op) {
    switch (op) {
    case '=':
      this->op = Operator::EQUAL;
      break;
    case '+':
      this->op = Operator::ADD;
      break;
    case '-':
      this->op = Operator::SUBTRACT;
      break;
    case '*':
      this->op = Operator::MULTIPLY;
      break;
    case '/':
      this->op = Operator::DIVIDE;
      break;
    case '^':
      this->op = Operator::POWER;
      break;
      // case '√':
      // this->op = Operator::ROOT;
      // break;
    }
  }
  NodeOperator(Operator op) : op(op) {}
};

struct NodeNumber {
  double value;
  friend std::ostream &operator<<(std::ostream &os, const NodeNumber &num) {
    os << num.value;
    return os;
  }
};

struct NodeVariable {
  std::string value;
  friend std::ostream &operator<<(std::ostream &os, const NodeVariable &var) {
    os << var.value;
    return os;
  }
};

typedef std::variant<NodeOperator, NodeNumber, NodeVariable> NodeValue;

struct Node {
  NodeType type;
  NodeValue value;
  Node *left;
  Node *right;
  Node() : left(nullptr), right(nullptr) {}
  Node(NodeType type, NodeValue value)
      : type(type), value(value), left(nullptr), right(nullptr) {}

  // allow std::cout to print the value
  friend std::ostream &operator<<(std::ostream &os, const Node &node) {
    switch (node.type) {
    case NodeType::OPERATOR:
      os << "Operator: " << std::get<NodeOperator>(node.value);
      break;
    case NodeType::NUMBER:
      os << "Number: " << std::get<NodeNumber>(node.value);
      break;
    case NodeType::VARIABLE:
      os << "Variable: " << std::get<NodeVariable>(node.value);
      break;
    }
    return os;
  }
};

class Formula {
public:
  Formula();
  Formula(std::string formula, FormType type);

  void print();

private:
  FormType formula_type; // not really needed! Or maybe it is?
  std::string formula_raw;
  std::string formula_tex;

  Node *root = nullptr;

  int iterator_depth = 0;
  Node *iterator;

  Node *parseString(const std::string &formula, Node *iterator, int depth = 0);
  Node *parseExpression(const std::string &expression, Node *iterator,
                        int depth = 0);
  Node *parseEquation(const std::string &equation, Node *iterator,
                      int depth = 0);
  // tree structure of the formula @todo
};

} // namespace Renderform

#endif // RENDERFORM_FORMULA_H
