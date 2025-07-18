#ifndef Move
#define Move
#include <stdint.h>

/**
 * move info:
 *  bits 0-5: from square (0 -> 63)
 *  bits 6-11: to square (0 -> 63)
 *  bits 12-14: flags (en passant, castling, promotion, etc.)
 * 
 * state_info:
 *  bits 0-3: castling legality
 *  bits 4-8: ep square (starting at 1, so 0 = no ep square)
 *  bits 9-11: captured piece
 *  bits 12...: fifty move counter
 */

struct game_state_stack_node {
    uint32_t board_state;
    uint16_t move;
    struct game_state_stack_node *prev_state;
};

typedef struct game_state_stack_node game_state;

enum flags {NONE, ENPASSANT, CASTLE, PROMOTE_QUEEN, PROMOTE_ROOK, PROMOTE_BISHOP, PROMOTE_KNIGHT, PAWN_TWO};

static const uint32_t CASTLING_MASK =   0b00000000000000000000000000001111;
static const uint32_t EP_MASK =         0b00000000000000000000000011110000;
static const uint32_t CAPTURE_MASK =    0b00000000000000000001111100000000;

// copy current state to a new node, 
game_state *push_move(game_state *cur);
// update most recent state as prev and return
game_state *pop_move(game_state *cur);

#endif