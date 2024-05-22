#include "formula.hpp"
#include <regex>

void PrettyPrintTree(Renderform::Node *root, int depth = 0, bool end = false) {
  if (root == nullptr) {
    return;
  }
  std::cout << depth << "  ";
  if (0 == depth) {
    std::cout << "*";
  }
  if (1 == depth) {
    if (end && root->right == nullptr && root->left == nullptr) {
      std::cout << "└─";
    } else {
      std::cout << "├─";
    }
  }
  for (int i = 1; i < depth; i++) {
    if (end && depth < 2) {
      std::cout << "└";
    } else {
      std::cout << "│";
    }
    std::cout << "  ";
  }
  for (int i = 1; i < depth; i++) {
    if (1 == i) {
      if (end) {
        std::cout << "└─";
      } else {
        std::cout << "├─";
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

void PrettyPrint(const std::string &text, int depth = 0, bool newline = true) {
  return; // NO DEBUGGING
  for (int i = 0; i < depth - 1; i++) {
    std::cout << "│  ";
  }
  std::cout << text;
  if (newline) {
    std::cout << std::endl;
  }
}

Renderform::Formula::Formula()
    : formula_raw(""), formula_type(FormType::EQUATION) {}

Renderform::Formula::Formula(std::string formula, FormType type)
    : formula_raw(formula), formula_type(type) {
  this->parseString(this->formula_raw, nullptr);
}

bool Renderform::Formula::areBracketsBalanced(const std::string &input) {
  std::stack<char> brackets;
  // check for balance (), [], {}

  for (char c : input) {
    if (c == '(' || c == '[' || c == '{') {
      brackets.push(c);
    } else if (c == ')' || c == ']' || c == '}') {
      if (brackets.empty()) {
        return false;
      }
      char top = brackets.top();
      if ((c == ')' && top == '(') || (c == ']' && top == '[') ||
          (c == '}' && top == '{')) {
        brackets.pop();
      } else {
        return false;
      }
    }
  }
  return brackets.empty();
}

/* Conceals (hides) conents of a group (i.e., (), [], {}) */
std::string Renderform::Formula::concealInnerGroup(const std::string &input) {
  std::string concealed;
  std::stack<char> brackets;
  for (char c : input) {
    if (c == '(' || c == '[' || c == '{') {
      if (brackets.empty()) {
        concealed += c;
      } else {
        concealed += " ";
      }
      brackets.push(c);
    } else if (c == ')' || c == ']' || c == '}') {
      char top = brackets.top();
      if ((c == ')' && top == '(') || (c == ']' && top == '[') ||
          (c == '}' && top == '{')) {
        brackets.pop();
        if (brackets.empty()) {
          concealed += c;
        } else {
          concealed += " ";
        }
      }
    } else {
      if (brackets.empty()) {
        concealed += c;
      } else {
        concealed += " ";
      }
    }
  }
  return concealed;
}

/*
Need to create a parse syntax tree for the formula
The syntax tree will be used to evaluate the formula or generate a LaTeX
representation of the formula

Need to use PEMDAS - parentheses, exponents, multiplication, division,
addition, subtraction (order of operations) Need to use associativity - left
to right or right to left (for example, division is left to right, so 6 / 2 /
3 = 1, not 2 / 3 = 0.667 and 6 / 0.667 = 9)

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
operator = "+" | "-" | "*" | "/" | "^" | "=" | ">" | "<" | ">=" | "<=" | "!="

*/

/*
Fatal error:
(3)(2) -
*/

Renderform::Node *Renderform::Formula::parseString(const std::string &formula,
                                                   Node *iterator, int depth) {
  std::string formula_formatted = formula;
  formula_formatted.erase(std::remove_if(formula_formatted.begin(),
                                         formula_formatted.end(), isspace),
                          formula_formatted.end());

  // Validate balanced parentheses/brackets/braces.
  if (!areBracketsBalanced(formula_formatted)) {
    PrettyPrint("Error: <formula> has unbalanced brackets.", depth);
    return nullptr;
  }

  Node *result = nullptr;
  if (formula_formatted.find("=") != std::string::npos) {
    result = this->parseEquation(formula_formatted, iterator, depth + 1);
  } else {
    result = this->parseExpression(formula_formatted, iterator, depth + 1);
  }
  if (nullptr == this->root) {
    this->root = result;
  }
  return result;
}

Renderform::Node *
Renderform::Formula::parseEquation(const std::string &equation, Node *iterator,
                                   int depth) {
  if (equation.find("=") == std::string::npos) {
    return nullptr;
  }

  PrettyPrint("└─ <equation>: " + equation, depth);

  // Split the equation into two sides
  // The left side is the left of the equals sign
  // The right side is the right of the equals sign
  std::string left_side = equation.substr(0, equation.find("="));
  std::string right_side = equation.substr(equation.find("=") + 1);

  Node *equals = Renderform::GetNodeEquals();
  if (nullptr == this->root) {
    this->root = equals;
  }

  // Parse the left side of the equation
  PrettyPrint("└─ <left_side>: ", depth + 1);
  equals->left = parseString(left_side, equals, depth + 1);
  PrettyPrint("└─ <right_side>: ", depth + 1);
  equals->right = parseString(right_side, equals, depth + 1);

  return equals;
}

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

Renderform::Node *
Renderform::Formula::parseExpression(const std::string &expression,
                                     Node *iterator, int depth) {
  std::string expression_outer =
      Renderform::Formula::concealInnerGroup(expression);

  if (expression != expression_outer) {
    PrettyPrint("(Outer expression groups hidden): " + expression_outer, depth);

    /*
    if outer has multiple () () following each other, then we need to treat it
    as multiplication implicitly e.g., (3)(2) = (3)*(2)
    */
    std::regex multiple_brackets_regex("\\)\\(");
    if (std::regex_search(expression, multiple_brackets_regex)) {
      std::string expression_outer_concealed =
          std::regex_replace(expression, multiple_brackets_regex, ")*(");
      return parseString(expression_outer_concealed, iterator, depth);
    }
  }

  Node *expr = nullptr;

  /*
  Need to determine if expression is only a variable/number (literal)
  or if it is a group (parentheses), unary (negative), or binary (operator)
  */

  /*
  Use regex to determine if the expression is a literal, group, unary, or
  binary
  */
  std::regex literal_regex("^[a-zA-Z0-9]+$");
  std::regex group_regex("^\\(.*\\)$");
  std::regex unary_regex("^-.+$");
  std::regex binary_regex(".+[\\+\\-\\*\\/\\^].+");

  if (std::regex_match(expression_outer, literal_regex)) {
    // Determine if the literal is a number or a variable
    if (std::regex_match(expression_outer, std::regex("^[0-9]+$"))) {
      PrettyPrint("└─ <expression[literal:number]>: " + expression, depth);
      expr = new Node{NodeType::NUMBER, NodeNumber{std::stod(expression)}};
    } else if (std::regex_match(expression_outer, std::regex("^[a-zA-Z]+$"))) {
      PrettyPrint("└─ <expression[literal:variable]>: " + expression, depth);
      expr = new Node{NodeType::VARIABLE, NodeVariable{expression}};
    } else if (std::regex_match(expression_outer,
                                std::regex("^[0-9]+[a-zA-Z]+$"))) {
      PrettyPrint("└─ <expression[literal:number*variable]>: " + expression,
                  depth);
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
      PrettyPrint("└─ <expression[literal:unknown_type]>: " + expression,
                  depth);
    }
  } else if (std::regex_match(expression_outer, binary_regex)) {
    PrettyPrint("└─ <expression[binary]>: " + expression, depth);
    // Use operator precedence to determine the operator
    // Split the expression into two parts
    // The operator is the middle part
    // The left side is the left of the operator
    // The right side is the right of the operator
    // Recursively parse the left and right sides

    // Find the operator with the lowest precedence which comes first

    // If valid parentheses are found, replace all inner contents with nothing
    // This will allow us to parse the inner contents first

    std::map<char, int> precedence = {{'^', 1},  // Exponentiation
                                      {'_', 1},  // Base
                                      {'*', 2},  // Multiplication
                                      {'/', 2},  // Division
                                      {'+', 3},  // Addition
                                      {'-', 3}}; // Subtraction
    int operator_precedence = 0;
    int operator_index = -1;

    for (int i = 0; i < expression_outer.length(); i++) {
      if (precedence.find(expression_outer[i]) != precedence.end()) {
        if (precedence[expression_outer[i]] >= operator_precedence) {
          operator_precedence = precedence[expression_outer[i]];
          operator_index = i;
        }
      }
    }

    if (operator_index == -1) {
      PrettyPrint("OPERATOR not found", depth);

      // Multiplication is implicit between parantheses and what follows
      // This is to allow for expressions like 2(3+4) to be valid or (2/3)3

      if (expression[0] == '(') {
        PrettyPrint("Implicit multiplication detected at end", depth);

        // find pos of closing bracket
        int closing_bracket =
            std::find(expression_outer.begin(), expression_outer.end(), ')') -
            expression_outer.begin();
        operator_index = closing_bracket + 1;

        // insert '*' at operator_index
        //
        std::cout << expression.substr(0, operator_index) << std::endl;
        std::cout << expression.substr(operator_index) << std::endl;

        Node *left = parseExpression(expression.substr(0, operator_index),
                                     iterator, depth + 1);
        Node *right = parseExpression(expression.substr(operator_index),
                                      iterator, depth + 1);
        expr = new Node{NodeType::OPERATOR, NodeOperator{Operator::MULTIPLY}};
        expr->left = left;
        expr->right = right;
        return expr;
      } else if (expression[expression.length() - 1] == ')') {
        PrettyPrint("Implicit multiplication detected at beginning", depth);

        // find pos of opening bracket
        int opening_bracket =
            std::find(expression_outer.begin(), expression_outer.end(), '(') -
            expression_outer.begin();
        operator_index = opening_bracket;

        // insert '*' at operator_index
        //
        std::cout << expression.substr(0, operator_index) << std::endl;
        std::cout << expression.substr(operator_index) << std::endl;

        Node *left = parseExpression(expression.substr(0, operator_index),
                                     iterator, depth + 1);
        Node *right = parseExpression(expression.substr(operator_index),
                                      iterator, depth + 1);
        expr = new Node{NodeType::OPERATOR, NodeOperator{Operator::MULTIPLY}};
        expr->left = left;
        expr->right = right;
        return expr;
      } else {
        PrettyPrint("Implicit multiplication NOT detected", depth);
        return nullptr;
      }
    }
    std::string operator_string =
        std::string(1, expression_outer[operator_index]);
    PrettyPrint("├─ <operator>: " + operator_string, depth);
    PrettyPrint("├─ <left_side>: " + expression.substr(0, operator_index),
                depth);
    Node *left = parseExpression(expression.substr(0, operator_index), iterator,
                                 depth + 1);
    PrettyPrint("├─ <right_side>: " + expression.substr(operator_index + 1),
                depth);
    Node *right = parseExpression(expression.substr(operator_index + 1),
                                  iterator, depth + 1);

    expr = new Node{NodeType::OPERATOR,
                    NodeOperator{expression_outer[operator_index]}};
    expr->left = left;
    expr->right = right;

  } else if (std::regex_match(expression_outer, group_regex)) {
    PrettyPrint("└─ <expression[group]>: " + expression, depth);
    // Remove outermost brackets (0, length-1)
    std::string inner_expression =
        expression.substr(1, expression.length() - 2);
    expr = parseExpression(inner_expression, iterator, depth + 1);
  } else if (std::regex_match(expression, unary_regex)) {
    PrettyPrint("└─ <expression[unary]>: " + expression, depth);

    expr = new Node{NodeType::NUMBER, NodeNumber{std::stod(expression)}};
  } else {
    // 2(3*x) is VALID -- 2*(3*x)
    // (3+2)2 is VALID -- (3+2)*2
    //
    int operator_index = -1;

    if (expression[0] == '(') {
      PrettyPrint("Implicit multiplication detected", depth);

      // find pos of closing bracket
      int closing_bracket =
          std::find(expression_outer.begin(), expression_outer.end(), ')') -
          expression_outer.begin();
      operator_index = closing_bracket + 1;

      // insert '*' at operator_index
      //
      std::cout << expression.substr(0, operator_index) << std::endl;
      std::cout << expression.substr(operator_index) << std::endl;

      Node *left = parseExpression(expression.substr(0, operator_index),
                                   iterator, depth + 1);
      Node *right = parseExpression(expression.substr(operator_index), iterator,
                                    depth + 1);
      expr = new Node{NodeType::OPERATOR, NodeOperator{Operator::MULTIPLY}};
      expr->left = left;
      expr->right = right;
      return expr;
    } else if (expression[expression.length() - 1] == ')') {
      PrettyPrint("Implicit multiplication detected", depth);

      // find pos of opening bracket
      int opening_bracket =
          std::find(expression_outer.begin(), expression_outer.end(), '(') -
          expression_outer.begin();
      operator_index = opening_bracket;

      // insert '*' at operator_index
      //
      std::cout << expression.substr(0, operator_index) << std::endl;
      std::cout << expression.substr(operator_index) << std::endl;

      Node *left = parseExpression(expression.substr(0, operator_index),
                                   iterator, depth + 1);
      Node *right = parseExpression(expression.substr(operator_index), iterator,
                                    depth + 1);
      expr = new Node{NodeType::OPERATOR, NodeOperator{Operator::MULTIPLY}};
      expr->left = left;
      expr->right = right;
      return expr;
    } else {
      PrettyPrint("Implicit multiplication NOT detected", depth);
      return nullptr;
    }
  }

  return expr;
}

void Renderform::Formula::print() {
  PrettyPrintTree(this->root);
  // std::cout << std::endl;
  // std::cout << "Tree Depth: " << this->root->getDepth() << std::endl;
  // std::cout << "Tree Nodes: " << this->root->getNumNodes() << std::endl;
}

// std::string Renderform::Formula::toLaTeX() {
//   std::string latex = "";
//   latex += "\\documentclass{article}\n";
//   latex += "\\usepackage{amsmath}\n";
//   latex += "\\begin{document}\n";
//   latex += "\\begin{equation}\n";
//   auto iterator = this->root;
// }

std::string Renderform::Formula::getLaTeX() { return getLaTeX(root); }

std::string Renderform::Formula::getLaTeX(Node *node) {
  if (!node)
    return "";

  switch (node->type) {
  case NodeType::OPERATOR: {
    NodeOperator op = std::get<NodeOperator>(node->value);
    std::string left = getLaTeX(node->left);
    std::string right = getLaTeX(node->right);

    switch (op.op) {
    case Operator::EQUAL:
      return left + " = " + right;
    case Operator::ADD:
      return left + " + " + right;
    case Operator::SUBTRACT:
      return left + " - " + right;
    case Operator::MULTIPLY:
      return left + " \\cdot " + right;
    case Operator::DIVIDE:
      return "\\frac{" + left + "}{" + right + "}";
    case Operator::POWER:
      return left + "^{" + right + "}";
    case Operator::ROOT:
      return "\\sqrt{" + right + "}";
    case Operator::FACTORIAL:
      return left + "!";
    default:
      return "";
    }
  }
  case NodeType::NUMBER: {
    NodeNumber num = std::get<NodeNumber>(node->value);

    // do not include .0000 in output
    if (num.value == (int)num.value)
      return std::to_string((int)num.value);
    else
      return std::to_string(num.value);
  }
  case NodeType::VARIABLE: {
    NodeVariable var = std::get<NodeVariable>(node->value);
    return var.value;
  }
  case NodeType::FUNCTION: {
    NodeFunction func = std::get<NodeFunction>(node->value);
    return func.name + "(" + getLaTeX(node->left) + ")";
  }
  default:
    return "";
  }
}
