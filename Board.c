#include "Board.h"
#include "Pieces.h"
#include <stdio.h>

// debugging
void print_bin(uint64_t board) {
    uint64_t x = 1ULL << (SQUARES - 1);
    while (x > 0) {
        printf((x & board)? "1": "0");
        x >>= 1;
    }
    printf("\n");
}
void print_board() {
    for (int i = 0; i < SQUARES; ++i)
        printf("%d. %d\n", i, board[i]);
}


uint64_t bitboards[2 * PIECE_TYPES] = {NONE};
uint64_t occupancy_board = NONE;
uint8_t board[SQUARES] = {NONE};

// int PIECE_COUNT = NONE;

const char *start_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
// rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R

void FEN_reader(const char *FEN) {
    if (FEN == NULL)
        FEN = start_FEN;

    int i = 0, start_sq = SQUARES - ROWS;
    for (int sq = start_sq; FEN[i] != ' '; ++i) {
        // printf("%d. %c \n", sq, FEN[i]);
        if (FEN[i] == '/') {
            sq = (start_sq -= ROWS);
        }
        else if (FEN[i] >= '1' && FEN[i] <= '8') {
            sq += FEN[i] - '0';
            // printf("%d empty squares\n", FEN[i] - '1');
        }
        else {
            int piece = get_piece_from_symbol(FEN[i]);
            int is_black = FEN[i] > 'a';
            
            uint64_t sq_bit = 1ULL << sq;

            bitboards[piece - 1 + PIECE_TYPES * is_black] |= sq_bit;
            occupancy_board |= sq_bit;
            board[sq] = make_piece(piece, BLACK * is_black + WHITE * !is_black);

            // printf("%c -> %d \n", FEN[i], board[sq]);

            // ++PIECE_COUNT;
            ++sq;
        }
    }

    // for (int i = 0; i < COLORED_PIECE_TYPES; ++i) 
    //     print_bin(bitboards[i]);

    // print_bin(occupancy_board);

    // print_board();
}
