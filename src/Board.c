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


uint64_t bitboards[2 * PIECE_TYPES] = {0};
uint64_t occupancy_board = 0;

uint8_t board[SQUARES] = {0};

int white_turn = 0;
int ply_count = 0;
int move_count = 0;
int fifty_counter = 0;

int ep_sq = 0;

int white_castle[2] = {0};
int black_castle[2] = {0};

game_state *cur_state = NULL;

const char *start_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void FEN_reader(const char *FEN) {
    if (FEN == NULL)
        FEN = start_FEN;

    int i = 0, start_sq = SQUARES - ROWS;
    for (int sq = start_sq; FEN[i] != ' '; ++i) {
        if (FEN[i] == '/') {
            sq = (start_sq -= ROWS);
        }
        else if (FEN[i] >= '1' && FEN[i] <= '8') {
            sq += FEN[i] - '0';
        }
        else {
            int piece = get_piece_from_symbol(FEN[i]);
            int is_black = FEN[i] > 'a';
            
            uint64_t sq_bit = 1ULL << sq;

            bitboards[piece - 1 + PIECE_TYPES * is_black] |= sq_bit;
            occupancy_board |= sq_bit;
            board[sq] = make_piece(piece, BLACK * is_black + WHITE * !is_black);
            ++sq;
        }
    }

    white_turn = (FEN[++i] == 'w');

    for (; FEN[i] != ' '; ++i) {
        if ('A' < FEN[i] < 'Z')
            white_castle[FEN[i] != 'K'] = 1;
        else if ('a' < FEN[i] < 'z')
            black_castle[FEN[i] != 'k'] = 1;
    }
    
    if (FEN[++i] != '-')
        ep_sq = alg_notation_to_sq(FEN[i], FEN[++i]);

    for (i += 2; FEN[i] != ' '; ++i)
        ply_count = ply_count * 10 + (FEN[i] - '0');

    for (++i; FEN[i] != ' '; ++i)
        move_count = move_count * 10 + (FEN[i] - '0');
    
    cur_state->board_state = fifty_counter << 14 | ep_sq << 4 | black_castle[1] << 3 | black_castle[0] << 2 | white_castle[1] << 1 | white_castle[0];
}

void undo_move(game_state *cur) {
    cur_state = pop_move(cur);


}
