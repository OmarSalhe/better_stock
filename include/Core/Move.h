#ifndef Move
#define Move

typedef unsigned int uint;
enum flags {NONE, ENPASSANT, CASTLE, PROMOTE_QUEEN, PROMOTE_ROOK, PROMOTE_BISHOP, PROMOTE_KNIGHT, PAWN_TWO_FORWARD};

#define SQUARE_MASK     0b0000000000000000000000111111U
#define FLAG_MASK       0b0000000000000111000000000000U
#define CASTLING_MASK   0b0000000001111000000000000000U
#define EP_MASK         0b0000111110000000000000000000U
#define CAPTURE_MASK    0b1111000000000000000000000000U

static inline uint move_made(uint start_sq, uint target_sq, uint flags) { return start_sq | target_sq << 6 | flags << 12; }
static inline uint start_sq(uint state) { return state & SQUARE_MASK; }
static inline uint target_sq(uint state) { return (state >> 6) & SQUARE_MASK; }
static inline uint captured_piece(uint state) { return (state & CAPTURE_MASK) >> 24; }
#endif