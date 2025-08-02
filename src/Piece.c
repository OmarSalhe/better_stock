#include "Pieces.h"

uint8_t get_piece_from_symbol(char symbol) {
    switch (symbol) {
        case 'p':
        case 'P': return PAWN;
        case 'n':
        case 'N': return KNIGHT;
        case 'b':
        case 'B': return BISHOP;
        case 'r':
        case 'R': return ROOK;
        case 'q':
        case 'Q': return QUEEN;
        case 'k':
        case 'K': return KING;
        default : return NONE;
    }
}

char get_symbol_from_piece(uint8_t piece) {
    switch (piece) {
        case PAWN:      return 'p';
        case KNIGHT:    return 'n';
        case BISHOP:    return 'b';
        case ROOK:      return 'r';
        case QUEEN:     return 'q';
        case KING:      return 'k';
        default:        return '\0';

    }
}