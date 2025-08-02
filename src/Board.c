#include "Board.h"
#include <stdlib.h>

// debugging
void print_board() {
    for (int i = 0; i < SQUARES; ++i)
        printf("%d. %d\n", i, board[i]);
}

void print_FEN_index(int i, const char *FEN, int line) {
    int n = 0;
    while (FEN[n] != '\0') ++n;
    printf((i >= n)? "%d / %d: index exceeds string length %d\n": "at index %d / %d in line %d\n", i, n, line);
}
#define print_index(i) print_FEN_index(i, FEN, __LINE__);

void print_state_variables() {
    for (int i = 0; i < 2; ++i)
        printf((!i)? "%d ": "%d\n", white_castle[i]);

    for (int i = 0; i < 2; ++i)
        printf((!i)? "%d ": "%d\n", black_castle[i]);

    for (int i = 0; i < 4; ++i) {
        if (i == 0)
                printf("white_turn: %d\n", white_turn);
        else if (i == 1)
                printf("ply_count: %d\n", ply_count);
        else if (i == 2)
                printf("move_count: %d\n", move_count);
        else if (i == 3)
                printf("ep_sq: %d\n", ep_sq);

    }
}
#define print_state() print_state_variables();

uint64_t bitboards[12] = {0};
uint64_t occupancy_board = 0;

uint8_t board[SQUARES] = {0};

int white_turn = 0;
int ply_count = 0;
int move_count = 0;

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

    white_turn = FEN[++i] == 'w';

    for (i += 2; FEN[i] != ' '; ++i) {
        if ('A' < FEN[i] && FEN[i] < 'Z')
            white_castle[FEN[i] != 'K'] = 1;

        else if ('a' < FEN[i] && FEN[i] < 'z')
            black_castle[FEN[i] != 'k'] = 1;
    }

    i++;
    ep_sq = (FEN[i] != '-') * alg_notation_to_sq(FEN[i], FEN[i + 1]);
    i += 1 + 2 * (FEN[i] != '-');
    
    if (FEN[++i] != '-')
        for (; FEN[i] != ' '; ++i)
            ply_count = ply_count * 10 + (FEN[i] - '0');

    if (FEN[++i] != '-')
        for (; FEN[i] != '\0'; ++i)
            move_count = move_count * 10 + (FEN[i] - '0');

    push_move(0U, 0U);
}

void pop_move() {
    if (cur_state->prev_state == NULL)
        return;
    // Set current position to prior position
    game_state *prev = cur_state->prev_state;
    
    // Update position info (i.e. castling, ep squares, etc.)
    white_castle[0] = prev->state_info >> 15 & 1;
    white_castle[1] = (prev->state_info >> 16 & 2) != 0;
    black_castle[0] = (prev->state_info >> 17 & 4) != 0;
    black_castle[1] = (prev->state_info >> 18 & 8) != 0;
    
    ep_sq = (prev->state_info & EP_MASK) >> 19;
    ply_count = prev->state_info >> 27;

    move_count -= white_turn;
    white_turn = !white_turn;
    
    // Replace pieces (i.e. undo move)
    board[start_sq(cur_state->state_info)] = board[target_sq(cur_state->state_info)];
    board[target_sq(cur_state->state_info)] = captured_piece(cur_state->state_info);
    
    free(cur_state);
    cur_state = prev;
}

void push_move(uint move_made, uint cap_piece) {
    // Append new state to stack
    game_state *new_state = malloc(sizeof(game_state));
    new_state->prev_state = cur_state;
    
    new_state->state_info = move_made | white_castle[0] << 15 | white_castle[1] << 16 | black_castle[0] << 17 |\
                            black_castle[1] << 18 | ep_sq << 19 | cap_piece << 24 | ply_count << 28;
    cur_state = new_state;
}