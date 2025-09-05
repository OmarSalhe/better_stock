#ifndef PIECES_H
#define PIECES_H
#include <stdint.h>

#define PIECE_TYPES 6   // Possible pieces (regardless of color)

#define NONE     ((uint8_t)0)
#define PAWN     ((uint8_t)1)
#define KNIGHT   ((uint8_t)2)
#define BISHOP   ((uint8_t)3)
#define ROOK     ((uint8_t)4)
#define QUEEN    ((uint8_t)5)
#define KING     ((uint8_t)6)

#define PAWN_VALUE       1 // Standard value of pawns
#define KNIGHT_VALUE     3 // Standard value of knights
#define BISHOP_VALUE     4 // Non-standard value of bishops (enforce preference towards bishop preservation)
#define ROOK_VALUE       5 // Standard value of rooks
#define QUEEN_VALUE      9 // Standard value of queens
#define KING_VALUE    9999 // Arbitrary value for kings (enforce preference towards checkmates)


// bit masks
#define TYPE_MASK     0b00000111 // bits 1-3 dedicated for piece types
#define COLOR_MASK    0b00001000 // bits 4 is dedicated for piece color


// colors
#define WHITE   0b00000000 // 4th bit = 0 if white
#define BLACK   0b00001000 // 4th bit = 1 if black


// available pieces
#define WHITE_PAWN       PAWN   | WHITE 
#define WHITE_KNIGHT     KNIGHT | WHITE
#define WHITE_BISHOP     BISHOP | WHITE
#define WHITE_ROOK       ROOK   | WHITE
#define WHITE_QUEEN      QUEEN  | WHITE
#define WHITE_KING       KING   | WHITE

#define BLACK_PAWN       PAWN   | BLACK
#define BLACK_KNIGHT     KNIGHT | BLACK
#define BLACK_BISHOP     BISHOP | BLACK
#define BLACK_ROOK       ROOK   | BLACK
#define BLACK_QUEEN      QUEEN  | BLACK
#define BLACK_KING       KING   | BLACK

// misc

// Returns 0 or 1 depending on piece color
static inline uint8_t is_white(uint8_t piece)                     { return (piece & COLOR_MASK) == WHITE && (piece != NONE); }

// returns piece color
static inline uint8_t piece_color(uint8_t piece)                  { return piece & COLOR_MASK; }

// returns piece type
static inline uint8_t piece_type(uint8_t piece)                   { return piece & TYPE_MASK; }

// returns a colored piece
static inline uint8_t make_piece(uint8_t piece, uint8_t color)    { return piece | color; }

uint8_t get_piece_from_symbol(char symbol);
char get_symbol_from_piece(uint8_t piece);

#endif