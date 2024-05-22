#include "formtree.hpp"

/*
Helpers for quickly creating nodes.
*/

Renderform::Node *Renderform::GetNodeEquals() {
  Renderform::Node *node = new Renderform::Node(
      Renderform::NodeType::OPERATOR,
      Renderform::NodeOperator(Renderform::Operator::EQUAL));
  return node;
}

Renderform::Node *Renderform::GetNodeAdd() {
  Renderform::Node *node =
      new Renderform::Node(Renderform::NodeType::OPERATOR,
                           Renderform::NodeOperator(Renderform::Operator::ADD));
  return node;
}

Renderform::Node *Renderform::GetNodeSubtract() {
  Renderform::Node *node = new Renderform::Node(
      Renderform::NodeType::OPERATOR,
      Renderform::NodeOperator(Renderform::Operator::SUBTRACT));
  return node;
}

Renderform::Node *Renderform::GetNodeMultiply() {
  Renderform::Node *node = new Renderform::Node(
      Renderform::NodeType::OPERATOR,
      Renderform::NodeOperator(Renderform::Operator::MULTIPLY));
  return node;
}

Renderform::Node *Renderform::GetNodeDivide() {
  Renderform::Node *node = new Renderform::Node(
      Renderform::NodeType::OPERATOR,
      Renderform::NodeOperator(Renderform::Operator::DIVIDE));
  return node;
}
