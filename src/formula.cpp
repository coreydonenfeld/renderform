#include "formula.hpp"

void PrettyPrintTree(Renderform::Node *root, int depth = 0, bool end = false) {
  if (root == nullptr) {
    return;
  }

  std::cout << "(" << depth << ") ";

  if (0 == depth) {
    std::cout << "*";
  }

  if (1 == depth) {
    if (end) {
      std::cout << "└";
    } else {
      std::cout << "├";
    }
  }

  for (int i = 1; i < depth; i++) {
    std::cout << "│";
    std::cout << " ";
  }

  for (int i = 1; i < depth; i++) {
    if (1 == i) {
      if (end) {
        std::cout << "└";
      } else {
        std::cout << "├";
      }
    } else {
      std::cout << "─";
    }
  }
  std::cout << " ";
  std::cout << *root << std::endl;
  PrettyPrintTree(root->left, depth + 1);
  PrettyPrintTree(root->right, depth + 1, true);
}

Renderform::Formula::Formula()
    : formula_raw(""), formula_type(FormType::EQUATION) {}

Renderform::Formula::Formula(std::string formula, FormType type)
    : formula_raw(formula), formula_type(type) {
  this->parseString(this->formula_raw, nullptr);
}

/*
Need to create a parse syntax tree for the formula
The syntax tree will be used to evaluate the formula or generate a LaTeX
representation of the formula

Need to use PEMDAS - parentheses, exponents, multiplication, division, addition,
subtraction (order of operations) Need to use associativity - left to right or
right to left (for example, division is left to right, so 6 / 2 / 3 = 1, not 2 /
3 = 0.667 and 6 / 0.667 = 9)

Doing basic things first like:
2 + 2 = 4
- =
-- +
--- 2
--- 2
-- 4

2 * 2 = 4
2 / 2 = 1
2 * 2 + 2 = 6
4 * 3 / 2 = 6
(2 + 12) * 3 = 42
*/

/*
expression = literal | group | unary | binary
literal = number | variable
group = "(" expression ")"
unary = "-" expression
binary = expression operator expression
operator = "+" | "-" | "*" | "/" | "^" | "="

*/

Renderform::Node *Renderform::Formula::parseString(const std::string &formula,
                                                   Node *iterator, int depth) {
  Node *result = nullptr;
  if (formula.find("=") != std::string::npos) {
    formula_type = FormType::EQUATION;
    result = this->parseEquation(formula, iterator);
  } else {
    formula_type = FormType::EXPRESSION;
    result = this->parseExpression(formula, iterator);
  }
  if (nullptr == this->root) {
    this->root = result;
  }
  return result;
}

Renderform::Node *
Renderform::Formula::parseEquation(const std::string &equation, Node *iterator,
                                   int depth) {

  // Split the equation into two sides
  // The left side is the left of the equals sign
  // The right side is the right of the equals sign
  std::string left_side = equation.substr(0, equation.find("="));
  std::string right_side = equation.substr(equation.find("=") + 1);

  Node *equals = new Node{NodeType::OPERATOR, NodeOperator{Operator::EQUAL}};

  if (nullptr == this->root) {
    this->root = equals;
  }

  // Node *two_node = new Node{NodeType::NUMBER, NodeNumber{2}};
  // Node *two_node_2 = new Node{NodeType::NUMBER, NodeNumber{2}};
  // Node *plus_node = new Node{NodeType::OPERATOR,
  // NodeOperator{Operator::ADD}};

  // Node *multiply_node =
  //     new Node{NodeType::OPERATOR, NodeOperator{Operator::MULTIPLY}};
  // Node *three_node = new Node{NodeType::NUMBER, NodeNumber{3}};
  // Node *twelve_node = new Node{NodeType::NUMBER, NodeNumber{12}};

  // Node *y_node = new Node{NodeType::VARIABLE, NodeVariable{"y"}};

  // equals->left = plus_node;
  // equals->right = y_node;
  // plus_node->left = multiply_node;
  // multiply_node->left = three_node;
  // multiply_node->right = twelve_node;
  // plus_node->right = two_node_2;

  // PrettyPrintTree(equals);

  // Parse the left side of the equation
  std::cout << "Left side: " << std::endl;
  equals->left = parseString(left_side, equals);
  std::cout << "Right side: " << std::endl;
  equals->right = parseString(right_side, equals);

  return equals;
}

Renderform::Node *
Renderform::Formula::parseExpression(const std::string &expression,
                                     Node *iterator, int depth) {
  std::cout << "-- Parsing expression: " << expression << std::endl;

  /*
  PEMDAS - parentheses, exponents, multiplication, division, addition,
  subtraction (order of operations)

  So if expression is 2 + 2 * 2
  We need to split recursively into:
  2 + (2 * 2)

  if input is:
  (2 + (2 * 2))

  we want iterator->left = 2
  iterator->right = (2 * 2)
  ... becomes ...
  iterator->right = *
  iterator->right->left = 2
  iterator->right->right = 2

  if input is:
  2 / (3 * (6 + 4))

  we want iterator->left = /
  iterator->left->left = 2
  iterator->left->right = (3 * (6 + 4))
  ... becomes ...
  iterator->left->right = *
  iterator->left->right->left = 3
  iterator->left->right->right = (6 + 4)
  ... becomes ...
  iterator->left->right->right = +
  iterator->left->right->right->left = 6
  iterator->left->right->right->right = 4
  */

  /*
  Need to determine if expression is only a variable/number (literal)
  or if it is a group (parentheses), unary (negative), or binary (operator)
  */

  Node *expr = nullptr;

  /*
  Use regex to determine if the expression is a literal, group, unary, or binary
  */
  std::regex literal_regex("^[a-zA-Z0-9]+$");
  std::regex group_regex("^\\(.*\\)$");
  std::regex unary_regex("^-.*$");
  std::regex binary_regex(".*[\\+\\-\\*\\/\\^].*");

  if (std::regex_match(expression, literal_regex)) {
    std::cout << "Literal: " << expression << std::endl;

    // Determine if the literal is a number or a variable
    if (std::regex_match(expression, std::regex("^[0-9]+$"))) {
      std::cout << "** Number: " << expression << std::endl;
      expr = new Node{NodeType::NUMBER, NodeNumber{std::stod(expression)}};
    } else if (std::regex_match(expression, std::regex("^[a-zA-Z]+$"))) {
      std::cout << "** Variable: " << expression << std::endl;
      expr = new Node{NodeType::VARIABLE, NodeVariable{expression}};
    } else if (std::regex_match(expression, std::regex("^[0-9]+[a-zA-Z]+$"))) {
      std::cout << "** Variable times num: " << expression << std::endl;
      std::string number = expression.substr(
          0, expression.find_first_of("abcdefghijklmnopqrstuvwxyz"));
      std::string variable = expression.substr(
          expression.find_first_of("abcdefghijklmnopqrstuvwxyz"));
      // Assumes only ONE number and ONE variable
      // 2ab assumes the variable is ab not a and b separately
      expr = new Node{NodeType::OPERATOR, NodeOperator{Operator::MULTIPLY}};
      expr->left = new Node{NodeType::NUMBER, NodeNumber{std::stod(number)}};
      expr->right = new Node{NodeType::VARIABLE, NodeVariable{variable}};
    } else {
      std::cout << "** Unknown: " << expression << std::endl;
    }
  } else if (std::regex_match(expression, group_regex)) {
    std::cout << "Group: " << expression << std::endl;
    // check open bracket has matching close bracket
    // remove outermost brackets and parse inner contents

    int open_bracket = 0;
    int close_bracket = 0;
    for (int i = 0; i < expression.length(); ++i) {
      if (expression[i] == '(') {
        open_bracket++;
      } else if (expression[i] == ')') {
        close_bracket++;
        open_bracket--;
      }
    }
    if (0 != open_bracket) {
      std::cout << "Error: Mismatched brackets" << std::endl;
      return nullptr;
    }
    // Remove outermost brackets (0, length-1)
    std::string inner_expression =
        expression.substr(1, expression.length() - 2);
    expr = parseExpression(inner_expression, iterator, depth + 1);
  } else if (std::regex_match(expression, binary_regex)) {
    std::cout << "Binary: " << expression << std::endl;
    // Use operator precedence to determine the operator
    // Split the expression into two parts
    // The operator is the middle part
    // The left side is the left of the operator
    // The right side is the right of the operator
    // Recursively parse the left and right sides

    // Find the operator with the lowest precedence which comes first

    // If valid parentheses are found, replace all inner contents with nothing
    // This will allow us to parse the inner contents first

    std::stack<int> parentheses_stack;
    std::string obscured_expression = expression;

    for (int i = 0; i < expression.length(); ++i) {
      if (expression[i] == '(') {
        parentheses_stack.push(i);
      } else if (expression[i] == ')') {
        if (!parentheses_stack.empty()) {
          int start = parentheses_stack.top();
          parentheses_stack.pop();
          for (int j = start + 1; j < i; ++j) {
            obscured_expression[j] = ' ';
          }
          std::cout << "Parentheses contents: "
                    << expression.substr(start + 1, i - start - 1) << std::endl;
        }
      }
    }

    std::cout << "Obscured Expression: " << obscured_expression << std::endl;

    std::map<char, int> precedence = {{'^', 1},  // Exponentiation
                                      {'*', 2},  // Multiplication
                                      {'/', 2},  // Division
                                      {'+', 3},  // Addition
                                      {'-', 3}}; // Subtraction
    int operator_precedence = 0;
    int operator_index = -1;

    for (int i = 0; i < obscured_expression.length(); i++) {
      if (precedence.find(obscured_expression[i]) != precedence.end()) {
        if (precedence[obscured_expression[i]] >= operator_precedence) {
          operator_precedence = precedence[obscured_expression[i]];
          operator_index = i;
        }
      }
    }

    if (operator_index == -1) {
      std::cout << "No operator found in expression: " << expression
                << std::endl;
      return nullptr;
    }
    std::cout << "Operator found: " << obscured_expression[operator_index]
              << std::endl;
    std::cout << "Left side: " << expression.substr(0, operator_index)
              << std::endl;
    std::cout << "Right side: " << expression.substr(operator_index + 1)
              << std::endl;

    Node *left = parseExpression(expression.substr(0, operator_index), iterator,
                                 depth + 1);
    Node *right = parseExpression(expression.substr(operator_index + 1),
                                  iterator, depth + 1);

    expr = new Node{NodeType::OPERATOR,
                    NodeOperator{obscured_expression[operator_index]}};
    expr->left = left;
    expr->right = right;

  } else if (std::regex_match(expression, unary_regex)) {
    std::cout << "Unary: " << expression << std::endl;
  }

  return expr;
}

void Renderform::Formula::print() { PrettyPrintTree(this->root); }
