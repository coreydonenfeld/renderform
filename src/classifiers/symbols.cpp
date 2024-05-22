#include "symbols.hpp"

std::pair<int, std::string> Renderform::getGroupAndSymbolFromNumber(int num) {
  if (0 <= num && 9 >= num) {
    return std::make_pair(0, std::to_string(num));
  } else if (10 <= num && 35 >= num) {
    std::string symbol;
    switch (num) {
    case SYMBOL_CHAR_A:
      symbol = "A";
      break;
    case SYMBOL_CHAR_B:
      symbol = "B";
      break;
    case SYMBOL_CHAR_C:
      symbol = "C";
      break;
    case SYMBOL_CHAR_D:
      symbol = "D";
      break;
    case SYMBOL_CHAR_E:
      symbol = "E";
      break;
    case SYMBOL_CHAR_F:
      symbol = "F";
      break;
    case SYMBOL_CHAR_G:
      symbol = "G";
      break;
    case SYMBOL_CHAR_H:
      symbol = "H";
      break;
    case SYMBOL_CHAR_I:
      symbol = "I";
      break;
    case SYMBOL_CHAR_J:
      symbol = "J";
      break;
    case SYMBOL_CHAR_K:
      symbol = "K";
      break;
    case SYMBOL_CHAR_L:
      symbol = "L";
      break;
    case SYMBOL_CHAR_M:
      symbol = "M";
      break;
    case SYMBOL_CHAR_N:
      symbol = "N";
      break;
    case SYMBOL_CHAR_O:
      symbol = "O";
      break;
    case SYMBOL_CHAR_P:
      symbol = "P";
      break;
    case SYMBOL_CHAR_Q:
      symbol = "Q";
      break;
    case SYMBOL_CHAR_R:
      symbol = "R";
      break;
    case SYMBOL_CHAR_S:
      symbol = "S";
      break;
    case SYMBOL_CHAR_T:
      symbol = "T";
      break;
    case SYMBOL_CHAR_U:
      symbol = "U";
      break;
    case SYMBOL_CHAR_V:
      symbol = "V";
      break;
    case SYMBOL_CHAR_W:
      symbol = "W";
      break;
    case SYMBOL_CHAR_X:
      symbol = "X";
      break;
    case SYMBOL_CHAR_Y:
      symbol = "Y";
      break;
    case SYMBOL_CHAR_Z:
      symbol = "Z";
      break;
    default:
      symbol = "ERROR";
      break;
    }
    return std::make_pair(1, symbol);
  } else if (36 <= num && 50 >= num) {
    std::string symbol;
    switch (num) {
    case SYMBOL_OP_EQUALS:
      symbol = "=";
      break;
    case SYMBOL_OP_NOTEQUALS:
      symbol = "!=";
      break;
    case SYMBOL_OP_LESSTHAN:
      symbol = "<";
      break;
    case SYMBOL_OP_LESSTHANEQUALS:
      symbol = "<=";
      break;
    case SYMBOL_OP_GREATERTHAN:
      symbol = ">";
      break;
    case SYMBOL_OP_GREATERTHANEQUALS:
      symbol = ">=";
      break;
    case SYMBOL_OP_PLUS:
      symbol = "+";
      break;
    case SYMBOL_OP_MINUS:
      symbol = "-";
      break;
    case SYMBOL_OP_TIMES:
      symbol = "×";
      break;
    case SYMBOL_OP_DIVIDE:
      symbol = "÷";
      break;
    case SYMBOL_OP_SQRT:
      symbol = "√";
      break;
    case SYMBOL_OP_FACTORIAL:
      symbol = "!";
      break;
    case SYMBOL_OP_PERCENT:
      symbol = "%";
      break;
    case SYMBOL_OP_EXPONENT:
      symbol = "^";
      break;
    case SYMBOL_OP_BASE:
      symbol = "_";
      break;
    }
    return std::make_pair(2, symbol);
  } else if (51 <= num && 59 >= num) {
    std::string symbol;
    switch (num) {
    case SYMBOL_GROUP_LPAREN:
      symbol = "(";
      break;
    case SYMBOL_GROUP_RPAREN:
      symbol = ")";
      break;
    case SYMBOL_GROUP_LBRACKET:
      symbol = "[";
      break;
    case SYMBOL_GROUP_RBRACKET:
      symbol = "]";
      break;
    case SYMBOL_GROUP_LBRACE:
      symbol = "{";
      break;
    case SYMBOL_GROUP_RBRACE:
      symbol = "}";
      break;
    case SYMBOL_GROUP_COMMA:
      symbol = ",";
      break;
    case SYMBOL_GROUP_DOT:
      symbol = ".";
      break;
    case SYMBOL_GROUP_COLON:
      symbol = ":";
      break;
    }
    return std::make_pair(3, symbol);
  }
  return std::make_pair(-1, "ERROR");
}
