#ifndef RENDERFORM_FORMULATREE_H
#define RENDERFORM_FORMULATREE_H

#include <iostream>
#include <map>
#include <regex>
#include <stack>
#include <string>
#include <variant>
#include <vector>

namespace Renderform {

// @see https://wumbo.net/operators/
enum class Operator {
  EQUAL,
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  POWER,
  ROOT,
  BASE,
  FACTORIAL,
};

/* @future Implement these functions */
enum class unused_Function {
  SIN,
  COS,
  TAN,
  ASIN,
  ACOS,
  ATAN,
  SINH,
  COSH,
  TANH,
  ASINH,
  ACOSH,
  ATANH,
  LOG,
  LN,
  SQRT,
  CBRT,
  ABS,
  EXP,
};

enum class NodeType { OPERATOR, NUMBER, VARIABLE, FUNCTION };

struct NodeOperator {
  Operator op;
  // Allow std::cout to print the value
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
    case Operator::BASE:
      os << "_";
      break;
    case Operator::FACTORIAL:
      os << "!";
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
    case '_':
      this->op = Operator::BASE;
      break;

      // case "√":
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

struct NodeFunction {
  std::string name;
  // allow std::cout to print the value
  friend std::ostream &operator<<(std::ostream &os, const NodeFunction &func) {
    os << func.name;
    return os;
  }
};

typedef std::variant<NodeOperator, NodeNumber, NodeVariable, NodeFunction>
    NodeValue;

struct Node {
  NodeType type;
  NodeValue value;
  Node *left;
  Node *right;
  Node() : left(nullptr), right(nullptr) {}
  Node(NodeType type, NodeValue value)
      : type(type), value(value), left(nullptr), right(nullptr) {}

  int getDepth() {
    if (left == nullptr && right == nullptr) {
      return 1;
    } else {
      return 1 + std::max(left->getDepth(), right->getDepth());
    }
  }

  int getNumNodes() {
    if (left == nullptr && right == nullptr) {
      return 1;
    } else {
      return 1 + left->getNumNodes() + right->getNumNodes();
    }
  }

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
    case NodeType::FUNCTION:
      os << "Function: " << std::get<NodeFunction>(node.value);
      break;
    }
    return os;
  }
};

Node *GetNodeEquals();
Node *GetNodeAdd();
Node *GetNodeSubtract();
Node *GetNodeMultiply();
Node *GetNodeDivide();

} // namespace Renderform

#endif // RENDERFORM_FORMULATREE_H
