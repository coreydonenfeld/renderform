#ifndef RENDERFORM_CLASSIFIER_SYMBOLS_HPP
#define RENDERFORM_CLASSIFIER_SYMBOLS_HPP

namespace Renderform {

// 0. DIGITS (0-9)
#define SYMBOL_DIGIT_0 0 // 0
#define SYMBOL_DIGIT_1 1 // 1
#define SYMBOL_DIGIT_2 2 // 2
#define SYMBOL_DIGIT_3 3 // 3
#define SYMBOL_DIGIT_4 4 // 4
#define SYMBOL_DIGIT_5 5 // 5
#define SYMBOL_DIGIT_6 6 // 6
#define SYMBOL_DIGIT_7 7 // 7
#define SYMBOL_DIGIT_8 8 // 8
#define SYMBOL_DIGIT_9 9 // 9

// 1. CHARACTERS (Alphabet)
#define SYMBOL_CHAR_A 10 // A, a
#define SYMBOL_CHAR_B 11 // B, b
#define SYMBOL_CHAR_C 12 // C, c
#define SYMBOL_CHAR_D 13 // D, d
#define SYMBOL_CHAR_E 14 // E, e
#define SYMBOL_CHAR_F 15 // F, f
#define SYMBOL_CHAR_G 16 // G, g
#define SYMBOL_CHAR_H 17 // H, h
#define SYMBOL_CHAR_I 18 // I, i
#define SYMBOL_CHAR_J 19 // J, j
#define SYMBOL_CHAR_K 20 // K, k
#define SYMBOL_CHAR_L 21 // L, l
#define SYMBOL_CHAR_M 22 // M, m
#define SYMBOL_CHAR_N 23 // N, n
#define SYMBOL_CHAR_O 24 // O, o
#define SYMBOL_CHAR_P 25 // P, p
#define SYMBOL_CHAR_Q 26 // Q, q
#define SYMBOL_CHAR_R 27 // R, r
#define SYMBOL_CHAR_S 28 // S, s
#define SYMBOL_CHAR_T 29 // T, t
#define SYMBOL_CHAR_U 30 // U, u
#define SYMBOL_CHAR_V 31 // V, v
#define SYMBOL_CHAR_W 32 // W, w
#define SYMBOL_CHAR_X 33 // X, x
#define SYMBOL_CHAR_Y 34 // Y, y
#define SYMBOL_CHAR_Z 35 // Z, z

// 2. OPERATORS
#define SYMBOL_OP_EQUALS 36            // =
#define SYMBOL_OP_NOTEQUALS 37         // !=
#define SYMBOL_OP_LESSTHAN 38          // <
#define SYMBOL_OP_LESSTHANEQUALS 39    // <=
#define SYMBOL_OP_GREATERTHAN 40       // >
#define SYMBOL_OP_GREATERTHANEQUALS 41 // >=
#define SYMBOL_OP_PLUS 42              // +
#define SYMBOL_OP_MINUS 43             // -
#define SYMBOL_OP_TIMES 44             // ×
#define SYMBOL_OP_TIMES_STAR 44        // *
#define SYMBOL_OP_TIMES_DOT 44         // ·
#define SYMBOL_OP_DIVIDE 45            // ÷
#define SYMBOL_OP_DIVIDE_SLASH 45      // /
#define SYMBOL_OP_SQRT 46              // √
#define SYMBOL_OP_FACTORIAL 47         // !
#define SYMBOL_OP_PERCENT 48           // %
#define SYMBOL_OP_EXPONENT 49          // ^
#define SYMBOL_OP_BASE 50              // _

// 3. GROUPS
#define SYMBOL_GROUP_LPAREN 51   // (
#define SYMBOL_GROUP_RPAREN 52   // )
#define SYMBOL_GROUP_LBRACKET 53 // [
#define SYMBOL_GROUP_RBRACKET 54 // ]
#define SYMBOL_GROUP_LBRACE 55   // {
#define SYMBOL_GROUP_RBRACE 56   // }
#define SYMBOL_GROUP_COMMA 57    // ,
#define SYMBOL_GROUP_DOT 58      // .
#define SYMBOL_GROUP_COLON 59    // :

/*
@future Implement more 'advanced' symbol categories:

CONSTANTS - pi, e, etc.
GREEKLETTERS - alpha, beta, etc.
MISC - infinity, degree, etc.
PROOFS - exists, forall, etc.
TRIG - sin, cos, tan, etc.
CALC - lim, int, sum, etc.
*/

} // namespace Renderform

#endif // RENDERFORM_CLASSIFIER_SYMBOLS_HPP
