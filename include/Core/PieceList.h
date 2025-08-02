#ifndef PieceList
#define PieceList

typedef struct piece_list {
    int num_pieces;
    int occupied_sq[16];
    int map[64];
} piece_list;

void add_piece(piece_list list, int sq);
void remove_piece(piece_list list, int sq);
void move_piece(piece_list list, int start_sq, int target_sq);

#endif