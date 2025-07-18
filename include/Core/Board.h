#ifndef Board
#define Board
#include <stdint.h>
#include "Pieces.h"
#include "Move.h"

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

// update state info to previous position's
void undo_move(game_state *cur);

void make_move(game_state *cur, int *ply_count, int *castling_rights, int *turn);

void FEN_reader(const char *position);

int alg_notation_to_sq(char file, char rank);
char *sq_to_alg_notation(int sq);

#endif