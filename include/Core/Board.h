#ifndef Board
#define Board
#include <stdint.h>
#include "Move.h"
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

extern int white_turn;
extern int ply_count;
extern int move_count;

extern int ep_sq;

extern int white_castle[2];
extern int black_castle[2];

extern game_state *cur_state;

// copy current state to a new node, 
void push_move(uint16_t move_made, uint8_t cap_piece);
// update most recent state as prev and return
void pop_move();

void FEN_reader(const char *position);

static inline int alg_notation_to_sq(char file, char rank) { return file - 'a' + ((rank - '1') << 3); }
// << 3 = * 8
// 1 << 3 = 1000 = 8, 2 << 3 = 10000 = 16, 3 << 3 = 11000 = 24, ...
char *sq_to_alg_notation(int sq);

#endif

