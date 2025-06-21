#ifndef Board
#define Board
#include <stdint.h>
#include "Pieces.h"

#define SQUARES 64  // Number of squares on a chess board
#define COLS 8      // Number of rows on a chess board
#define ROWS 8      // Number of columns on a chess board

// const uint8_t QUEEN_CASTLE = 1;
// const uint8_t KING_CASTLE =  2;
// const uint8_t WHITES_TURN =  4;

/**
 * White:
 *  PAWN
 *  KNIGHT
 *  BISHOP
 *  ROOK
 *  QUEEN
 *  KING
 * 
 * Black:
 *  PAWN
 *  KNIGHT
 *  BISHOP
 *  ROOK
 *  QUEEN
 *  KING
 */

extern uint64_t bitboards[];
extern uint64_t occupancy_board;
extern uint8_t board[];

// extern int PIECE_COUNT;

// uint8_t game_info;
// int whites_turn;
// int castling_rights[];

static inline int is_light_square(int row, int col) {
    return (row + col) & 1;
}

void FEN_reader(const char *position);


#endif