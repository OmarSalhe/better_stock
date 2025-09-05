#ifndef MoveData
#define MoveData
#include <stdint.h>

/**
 *  MoveData:
 *      piece offsets (if applicable) -> all directions
 *      piece attacks (bpawn, wpawn, rook, king, knight, d bishop, l bishop) -> bitboards, squares, or both? (squares means iterating through possible attacked squares, can i instead encode attacked squares into bit string? just AND attacked square to bit string to see if valid move. => easier to iterate through moves when simulating )
 *      need to know edges of board (for sliding pieces)
 * 
 * 
 *  magic bit boards
 *      count how many squares a sliding piece can reach
 *      mask attacked squares
 *      multiply by a number found after simulating a bunch of numbers
 *      shift product by 64 - number of attacked squares -> gives index of relevant bitboard
 * 
 *  make array for all possible piece configs (multiple blockers included or just one though?)
 *      just get board and for every square mark all after as 0
 * 
 * 
 * 
 * only use quarter of board
 *      if square is quad 2 -> rotate left once (y is 0 away from end so go 0 away from start -> 0 )
 *      ...
 * 
 * 
 */

extern uint64_t pawn_capture_bitboard[][2];
extern uint64_t knight_bitboard[];
extern uint64_t king_bitboard[];
extern uint64_t bishop_bitboard[];
extern uint64_t rook_bitboard[];

// contains a list of squares reachable by pieces
extern int pawn_captures[][2][2];
extern int knight_moves[][8];
extern int king_moves[][8];

extern uint64_t* attack_table[];
extern uint64_t magic_table[];
#endif