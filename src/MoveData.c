#include "MoveData.h"
#include "MagicBitboard.h"
#include <stdio.h>
#include <stdlib.h>

#define sq_to_edge num_square_to_edge
#define occupied_sq num_squares_reached

#define N 8
#define S -8
#define E 1
#define W -1

#define ROOK_OFFSET 64

#define min(x, y) (((y) > (x))? (x): (y))
#define row(x) ((x) >> 3)
#define col(x) ((x) & 0b111)
#define on_edge(x) (row(x) == 0 || row(x) == 7 || col(x) == 0 || col(x) == 7)

#define ROOK_OFFSET 64

#define print_bin _print_binary_board_form
void _print_binary_board_form(uint64_t board, int square) {
    printf("%c%d\n", 'a' + col(square), 1 + row(square));
    for (int r = 7, f = 0; r >= 0; r -= (f == 7), f = (f + 1) * (f != 7)) 
        printf("%3d %s", (board & 1ULL << (f + r * 8))? 1: 0, (f == 7)? "\n\n": "\0");

    printf("\n");
}

void init_move_stuff(void) {
    const int direction[] = {N, S, E, W, N + E, N + W, S + E, S + W};
    
    
    int sq_to_edge[64][8];
    
    for (int sq = 0; sq < 64; ++sq) {
        int board_x = col(sq);
        int board_y = row(sq);
        
        int north = 8 - board_y - 1;
        int south = board_y;
        int east = 8 - board_x - 1;
        int west = board_x;
        
        sq_to_edge[sq][0] = north;
        sq_to_edge[sq][1] = south;
        sq_to_edge[sq][2] = east;
        sq_to_edge[sq][3] = west;
        sq_to_edge[sq][4] = min(north, east);
        sq_to_edge[sq][5] = min(north, west);
        sq_to_edge[sq][6] = min(south, east);
        sq_to_edge[sq][7] = min(south, west);

        // for (int dir = 0; dir < 8; ++dir)
        //     printf("%3d ", sq_to_edge[sq][dir]);
        // printf("\n\n");
    }

    const uint64_t a_file = 0b0000000100000001000000010000000100000001000000010000000100000001;
    const uint64_t h_file = 0b1000000010000000100000001000000010000000100000001000000010000000;
    
    // Pawn stuff
    const int pawn_attack_offset[2][2] = {
        {S + E, S + W},
        {N + E, N + W}
    };
    
    uint64_t pawn_capture_bitboard[64][2];
    int pawn_captures[64][2][2];
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
        
        int ne_sq = sq + pawn_attack_offset[1][1], nw_sq = sq + pawn_attack_offset[1][0];
        int se_sq = sq + pawn_attack_offset[0][0], sw_sq = sq + pawn_attack_offset[0][0];
        uint64_t b_east_cap = -(uint64_t)(se_sq >= 0) & (1ULL << (se_sq)) & ~a_file;
        uint64_t b_west_cap = -(uint64_t)(sw_sq >= 0) & (1ULL << (sw_sq)) & ~h_file;
        
        uint64_t w_east_cap = -(uint64_t)(ne_sq < 64) & (1ULL << (ne_sq)) & ~a_file;
        uint64_t w_west_cap = -(uint64_t)(nw_sq < 64) & (1ULL << (nw_sq)) & ~h_file;
        
        pawn_capture_bitboard[sq][1] = w_east_cap | w_west_cap;
        pawn_capture_bitboard[sq][0] = b_east_cap | b_west_cap;

        pawn_captures[sq][1][0] = (w_east_cap > 0) * ne_sq - !w_east_cap;
        pawn_captures[sq][1][1] = (w_west_cap > 0) * nw_sq - !w_west_cap;
        pawn_captures[sq][0][0] = (b_east_cap > 0) * se_sq - !b_east_cap;
        pawn_captures[sq][0][1] = (b_west_cap > 0) * sw_sq - !b_west_cap;

        // print_bin(bit[sq][0], sq);
        // printf("\n");
    }
    
    
    // Knight stuff
    const int knight_offset[] = {2 * N + E, 2 * N + W, 2 * S + E, 2 * S + W, 2 * E + N, 2 * E + S, 2 * W + N, 2 * W + S};
    uint64_t knight_bitboard[64];
    int knight_moves[64][9];

    for (int sq = 0; sq < 64; ++sq) {
        /**
         * two's-compleemnt negation
         * 
         * -(uint64_t)(...) -> ???
         *  
         * (...) evaluates whether or not a knight can jump to a specific square from where it's at
         * (uint64_t) casts the result (0 or 1) into an unsigned 64-bit number since we're operating with those
         * 
         * unsigned numbers wrap when exceeding or going below its defined range (i.e. [0, 2^64) ), so 0 - 1 = 2^64 - 1 and (2^64 - 1) + 1 = 0
         * if a square can be reached, the condition should result in 1, signifying the square bit should be kept.
         * a way to express this is mask it using a mask of 1s, which is equal to 2^64 - 1 for 64-bit integers
         * since 0 - 1 = 2^64 - 1 and 0 - 1 = -1 (typically), keeping a bit can then be determined by the negation of a squares accessibility from the current square
         * 
         * i.e. valid sq = 1 -> (-1) & sq returns the same square
         *      invalid sq = 0 -> (-0) & sq returns 0, which is effectively nothing
         */

        uint64_t north_east = -(uint64_t)(sq_to_edge[sq][0] >= 2 && sq_to_edge[sq][2] >= 1) & 1ULL << (sq + knight_offset[0]);
        uint64_t north_west = -(uint64_t)(sq_to_edge[sq][0] >= 2 && sq_to_edge[sq][3] >= 1) & 1ULL << (sq + knight_offset[1]);
        
        uint64_t south_east = -(uint64_t)(sq_to_edge[sq][1] >= 2 && sq_to_edge[sq][2] >= 1) & 1ULL << (sq + knight_offset[2]);
        uint64_t south_west = -(uint64_t)(sq_to_edge[sq][1] >= 2 && sq_to_edge[sq][3] >= 1) & 1ULL << (sq + knight_offset[3]);
        
        uint64_t east_north = -(uint64_t)(sq_to_edge[sq][2] >= 2 && sq_to_edge[sq][0] >= 1) & 1ULL << (sq + knight_offset[4]);
        uint64_t east_south = -(uint64_t)(sq_to_edge[sq][2] >= 2 && sq_to_edge[sq][1] >= 1) & 1ULL << (sq + knight_offset[5]);
        
        uint64_t west_north = -(uint64_t)(sq_to_edge[sq][3] >= 2 && sq_to_edge[sq][0] >= 1) & 1ULL << (sq + knight_offset[6]);
        uint64_t west_south = -(uint64_t)(sq_to_edge[sq][3] >= 2 && sq_to_edge[sq][1] >= 1) & 1ULL << (sq + knight_offset[7]);
        
        knight_bitboard[sq]= north_east | north_west | south_east | south_west | east_north | east_south | west_north | west_south;
        knight_moves[sq][0] = (north_east > 0) * (N + E + sq) - !north_east;
        knight_moves[sq][1] = (north_west > 0) * (N + W + sq) - !north_west;
        knight_moves[sq][2] = (south_east > 0) * (S + E + sq) - !south_east;
        knight_moves[sq][3] = (south_west > 0) * (S + W + sq) - !south_west;
        knight_moves[sq][4] = (east_north > 0) * (E + N + sq) - !east_north;
        knight_moves[sq][5] = (east_south > 0) * (E + S + sq) - !east_south;
        knight_moves[sq][6] = (west_north > 0) * (W + N + sq) - !west_north;
        knight_moves[sq][7] = (west_south > 0) * (W + S + sq) - !west_south;
    }
    
    // King stuff
    uint64_t king_bitboard[64] = {0};
    int king_moves[64][9];
    for (int sq = 0; sq < 64; ++sq) {
        for (int dir = 0, i = 0; dir < 8; ++dir) {
            int is_legal = sq_to_edge[sq][dir] != 0;
            king_bitboard[sq] |= (1ULL << (sq + direction[dir])) & -(uint64_t)(is_legal);
            king_moves[sq][i] = is_legal * (sq + direction[dir]) - !is_legal;
        }

        // print_bin(king_bitboard[sq], sq);
    }

    // Sliding pieces
    uint64_t bishop_bitboard[64] = {0};
    uint64_t rook_bitboard[64] = {0};
    int occupied_sq[64][2];
    for (int sq = 0; sq < 64; ++sq) {
        for (int dir = 0; dir < 8; ++dir) {
            occupied_sq[sq][dir < 4] += sq_to_edge[sq][dir] - 1;
            uint64_t *board = (dir < 4)? rook_bitboard : bishop_bitboard;
            for (int n = 1; n < sq_to_edge[sq][dir]; ++n)
                board[sq] |= 1ULL << (sq + n * direction[dir]);
            
        }
        print_bin(bishop_bitboard[sq], sq);
    }

    // where the **MAGIC** happens
    uint64_t* attack_table[128];
    uint64_t magic_table[64];
    for (int sq = 0; sq < 64; ++sq) {
        if((attack_table[sq] = malloc(sizeof(uint64_t) * (1ULL << occupied_sq[sq][0]))) == NULL) {
            printf("Error allocating memory to attack table\n");
            return -1;
        }
        if ((attack_table[sq + ROOK_OFFSET] = malloc(sizeof(uint64_t) * (1 << occupied_sq[sq][1]))) == NULL){
            printf("Error allocating memory to attack table\n");
            return -1;
        }

        
    }
}

int main(void) {
    init_move_stuff();
    return -1;
}
