#include "MoveData.h"

#define N 8
#define S -8
#define E 1
#define W -1

#define min(x, y) (((y) > (x))? (x): (y))

void init_move_stuff(void) {
    const int direction[] = {N, S, E, W, N + E, N + W, S + E, S + W};
    
    
    int sq_to_edge[64][8];
    
    for (int sq = 0; sq < 64; ++sq) {
        int board_x = sq & 0b111;
        int board_y = sq >> 3;
        
        int north = 8 - board_y;
        int south = board_y;
        int east = 8 - board_x;
        int west = board_x;
        
        sq_to_edge[sq][0] = north;
        sq_to_edge[sq][1] = south;
        sq_to_edge[sq][2] = east;
        sq_to_edge[sq][3] = west;
        sq_to_edge[sq][4] = min(north, east);
        sq_to_edge[sq][5] = min(north, west);
        sq_to_edge[sq][6] = min(south, east);
        sq_to_edge[sq][7] = min(south, west);
    }
    
    const uint64_t a_file = 0b0000000100000001000000010000000100000001000000010000000100000001;
    const uint64_t h_file = 0b1000000010000000100000001000000010000000100000001000000010000000;
    
    // Pawn stuff
    const int pawn_attack_offset[2][2] = {
        {S + E, S + W},
        {N + E, N + W}
    };

    uint64_t pawn_capture_board[64][2];
    for (int sq = 0; sq < 64; ++sq) {
        /**
         * ~a_file, ~h_file
         *      since using bitboards, no explicit ways to prevent attacks from wrapping around the board
         *      a workaround is to mask off the a-file on any eastward captures and h-file for westward captures
         *      (since going northeast should not lead to a northwest capture and viceversa)
         * 
         *              ~a_file (board perspective)
         *           0   1   1   1   1   1   1   1
         * 
         *           0   1   1   1   1   1   1   1
         * 
         *           0   1   1   1   1   1   1   1
         * 
         *           0   1   1   1   1   1   1   1
         * 
         *           0   1   1   1   1   1   1   1
         * 
   but goes here  <- 0   1   1   1   1   1   1   1
instead and gets masked off
         *           0   1   1   1   1   1   1   1   X  -> should go here
         * 
         *           0   1   1   1   1   1   1   X -> a capture here
         * 
         *      
         */
        
        uint64_t b_east_cap = ((sq + pawn_attack_offset[0][0] < 64)? 1ULL << (sq + pawn_attack_offset[0][0]): 0) & ~a_file;
        uint64_t b_west_cap = ((sq + pawn_attack_offset[0][1] < 64)? 1ULL << (sq + pawn_attack_offset[0][1]): 0) & ~h_file;

        uint64_t w_east_cap = ((sq + pawn_attack_offset[1][0] >= 0)? 1ULL << (sq + pawn_attack_offset[1][0]): 0) & ~a_file;
        uint64_t w_west_cap = ((sq + pawn_attack_offset[1][1] >= 0)? 1ULL << (sq + pawn_attack_offset[1][1]): 0) & ~h_file;

        pawn_capture_board[sq][1] = w_east_cap | w_west_cap;
        pawn_capture_board[sq][0] = b_east_cap | b_west_cap;
    }


    // Knight stuff
    const int knight_offset[] = {2 * N + E, 2 * N + W, 2 * S + E, 2 * S + W, 2 * E + N, 2 * E + S, 2 * W + N, 2 * W + S};
    uint64_t knight_move_board[64];
    for (int sq = 0; sq < 64; ++sq) {
        uint64_t north_east = ((sq + knight_offset[0] < 64)? 1ULL << (sq + knight_offset[0]): 0) & ~a_file;
        uint64_t north_west = ((sq + knight_offset[1] >= 0)? 1ULL << (sq + knight_offset[1]): 0) & ~h_file;

        uint64_t south_east = ((sq + knight_offset[2] < 64)? 1ULL << (sq + knight_offset[2]): 0) & ~a_file;
        uint64_t south_west = ((sq + knight_offset[3] >= 0)? 1ULL << (sq + knight_offset[3]): 0) & ~h_file;

        uint64_t east_north = ((sq + knight_offset[4] < 64)? 1ULL << (sq + knight_offset[4]): 0) & ~a_file;
        uint64_t east_south = ((sq + knight_offset[5] >= 0)? 1ULL << (sq + knight_offset[5]): 0) & ~a_file;
        
        uint64_t west_north = ((sq + knight_offset[6] < 64)? 1ULL << (sq + knight_offset[6]): 0) & ~h_file;
        uint64_t west_south = ((sq + knight_offset[7] >= 0)? 1ULL << (sq + knight_offset[7]): 0) & ~h_file;
        
        knight_move_board[sq]= north_east | north_west | south_east | south_west | east_north | east_south | west_north | west_south;
    }
}
