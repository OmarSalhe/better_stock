#include "Move.h"

game_state *pop_move(game_state *cur) {
    game_state *prev = cur->prev_state;
    free(cur);

    return prev;
}

game_state *push_move(game_state *cur, uint32_t next_state) {
    game_state *new_state = malloc(sizeof(game_state));

    new_state->board_state = next_state;
    new_state->prev_state = cur;

    return new_state;
}

