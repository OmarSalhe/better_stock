#ifndef Pieces
#define Pieces
#include <stdint.h>

#define PIECE_TYPES         6   // Possible pieces (regardless of colors)
#define COLORED_PIECE_TYPES 12  // Possible pieces (considering colors)

// Types of chess pieces
static const uint8_t NONE =     0U;
static const uint8_t PAWN =     1U;
static const uint8_t KNIGHT =   2U;
static const uint8_t BISHOP =   3U;
static const uint8_t ROOK =     4U;
static const uint8_t QUEEN =    5U;
static const uint8_t KING =     6U;

static const int PAWN_VALUE =      1; // Standard value of pawns
static const int KNIGHT_VALUE =    3; // Standard value of knights
static const int BISHOP_VALUE =    4; // Non-standard value of bishops (enforce preference towards bishops positions)
static const int ROOK_VALUE =      5; // Standard value of rooks
static const int QUEEN_VALUE =     9; // Standard value of queens
static const int KING_VALUE =   9999; // Arbitrary value for kings (enforce preference towards checkmates)


// bit masks
static const uint8_t TYPE_MASK =       0b00000111; // bits 1-3 dedicated for piece types
static const uint8_t COLOR_MASK =      0b00001000; // bits 4 is dedicated for piece color


// colors
static const uint8_t WHITE = 0b00000000; // 4th bit = 0 if white
static const uint8_t BLACK = 0b00001000; // 4th bit = 1 if black


// available pieces
static const uint8_t WHITE_PAWN =      PAWN   | WHITE; 
static const uint8_t WHITE_KNIGHT =    KNIGHT | WHITE;
static const uint8_t WHITE_BISHOP =    BISHOP | WHITE;
static const uint8_t WHITE_ROOK =      ROOK   | WHITE;
static const uint8_t WHITE_QUEEN =     QUEEN  | WHITE;
static const uint8_t WHITE_KING =      KING   | WHITE;

static const uint8_t BLACK_PAWN =      PAWN   | BLACK;
static const uint8_t BLACK_KNIGHT =    KNIGHT | BLACK;
static const uint8_t BLACK_BISHOP =    BISHOP | BLACK;
static const uint8_t BLACK_ROOK =      ROOK   | BLACK;
static const uint8_t BLACK_QUEEN =     QUEEN  | BLACK;
static const uint8_t BLACK_KING =      KING   | BLACK;

// misc

// Returns 0 or 1 depending on piece color
static inline uint8_t is_white(uint8_t piece)                     { return piece & COLOR_MASK && piece != NONE; };

// returns piece color
static inline uint8_t piece_color(uint8_t piece)                  { return piece & COLOR_MASK; };

// returns piece type
static inline uint8_t piece_type(uint8_t piece)                   { return piece & TYPE_MASK; };

// returns a colored piece
static inline uint8_t make_piece(uint8_t piece, uint8_t color)    { return piece | color; };

static uint8_t get_piece_from_symbol(char symbol) {
    switch (symbol) {
        case 'p':
        case 'P': return PAWN;
        case 'n':
        case 'N': return KNIGHT;
        case 'b':
        case 'B': return BISHOP;
        case 'r':
        case 'R': return ROOK;
        case 'q':
        case 'Q': return QUEEN;
        case 'k':
        case 'K': return KING;
        default : return NONE;
    }
}

static char get_symbol_from_piece(uint8_t piece) {
    switch (piece) {
        case PAWN:      return 'p';
        case KNIGHT:    return 'n';
        case BISHOP:    return 'b';
        case ROOK:      return 'r';
        case QUEEN:     return 'q';
        case KING:      return 'k';
        default:        return '\0';

    }
}

#endif