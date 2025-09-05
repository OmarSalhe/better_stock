#ifndef MAGIC_BITBOARD_H
#define MAGIC_BITBOARD_H
#include <stdint.h>
#include <stdlib.h>

typedef struct magic_entry {
    uint64_t mask;
    uint64_t magic;
    int index_bits;
    int offset;
};

int random_uint64();
int 
int find_magic(uint64_t *table, int size);

#endif