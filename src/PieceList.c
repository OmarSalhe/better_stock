#include "PieceList.h"

void add_piece(piece_list list, int sq) {
    list.map[sq] = list.num_pieces;
    list.occupied_sq[list.num_pieces++] = sq;
}

void remove_piece(piece_list list, int sq) {
    list.occupied_sq[list.map[sq]] = list.occupied_sq[--list.num_pieces]; // replace removed piece with existing piece
    list.map[list.occupied_sq[list.map[sq]]] = list.map[sq]; // save moved piece's new index in occupied_sq to the map
}

void move_piece(piece_list list, int start_sq, int target_sq) {
    list.occupied_sq[list.map[start_sq]] = target_sq; // update previously occupied square to new square
    list.map[target_sq] = list.map[start_sq]; // save on map what index occupies target_sq
}