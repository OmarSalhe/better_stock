#ifndef Move
#define Move

typedef unsigned int uint;

/**
 * move info:
 *  bits 0-5: from square (0 -> 63)
 *  bits 6-11: to square (0 -> 63)
 *  bits 12-14: flags (en passant, castling, promotion, etc.)
 * 
 * state_info:
 *  bits 15-18: castling legality (0: w king, 1: w queen, 2: b king, 3: b queen)
 *  bits 19-23: ep square (starting at 1, so 0 = no ep square)
 *  bits 24-26: captured piece
 *  bits 27...: fifty move counter
 */

struct game_state_stack_node {
    uint state_info;
    struct game_state_stack_node *prev_state;
};

typedef struct game_state_stack_node game_state;

enum flags { NONE, ENPASSANT, CASTLE, PROMOTE_QUEEN, PROMOTE_ROOK, PROMOTE_BISHOP, PROMOTE_KNIGHT, PAWN_TWO_FORWARD };

static const uint SQUARE_MASK =     0b000000000000000000000111111;
static const uint FLAG_MASK =       0b000000000000111000000000000;
static const uint CASTLING_MASK =   0b000000001111000000000000000;
static const uint EP_MASK =         0b000111110000000000000000000;
static const uint CAPTURE_MASK =    0b111000000000000000000000000;


#endif