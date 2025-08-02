#ifndef Board
#define Board
#include <stdint.h>
#include <stdio.h>
#include "Move.h"
#include "Pieces.h"
#include "PieceList.h"

#define SQUARES 64  // Number of squares on a chess board
#define COLS 8      // Number of rows on a chess board
#define ROWS 8      // Number of columns on a chess board

// debugging
#define print_bin(bin)\
{\
    uint64_t x = 1ULL << (SQUARES - 1);\
    int bit = 1;\
    while (x > 0) {\
        printf((x & bin)? "1": "0");\
        printf((bit++ % 8 == 0)? " ": "");\
        x >>= 1;\
    }\
    printf("\n");\
}\

typedef unsigned int uint;
/**
 * move info:
 *  bits 0-5: from square (0 -> 63)
 *  bits 6-11: to square (0 -> 63)
 *  bits 12-14: flags (en passant, castling, promotion, etc.)
 * 
 * state_info:
 *  bits 15-18: castling legality (15: w king, 16: w queen, 17: b king, 18: b queen)
 *  bits 19-23: ep square (starting at 1, so 0 = no ep square)
 *  bits 24-27: captured piece
 *  bits 28...: fifty move counter
 */

struct game_state_stack_node {
    uint state_info;
    struct game_state_stack_node *prev_state;
};
typedef struct game_state_stack_node game_state;

extern uint64_t bitboards[];
extern uint64_t occupancy_board;

extern uint8_t board[];

extern int white_turn; // 1 = whites turn, 0 = black turn
extern int ply_count;  // number of half-moves made since capture/pawn move (for fifty-move counter)
extern int move_count; // number of moves made (increments after black) 

extern int ep_sq; // en passant square

extern int white_castle[2]; // represents white castling legality on either side; i = 0: king-side, i = 1: queen-side
extern int black_castle[2]; // represents black castling legality

extern game_state *cur_state; // holds move made to reach position, state information, and the prior position

// adds position onto a history stack
void push_move(uint move_made, uint cap_piece);

// updates the position to the position prior
void pop_move();

// initializes a position from a FEN string
void FEN_reader(const char *position);

static inline int file_index(int sq) { return sq & 0b111; }
static inline int rank_index(int sq) { return sq >> 3; }
static inline int alg_notation_to_sq(char file, char rank) { return file - 'a' + ((rank - '1') << 3); }

char *sq_to_alg_notation(int sq);

#endif

