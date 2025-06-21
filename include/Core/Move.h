#ifndef Move
#define Move

    struct {
        int piece;
        int taken;
        int from;
        int to;
        int flags; // castling rights, en passant square, 
        int half_move; // designated variable for cases where half-move count resets

        /**
         * 
         * w + s -> w => white && even turns -> white
         * w + d -> b => white && odd turns -> black
         * b + s -> b => black && even turns -> black
         * b + d -> w => black and odd turns -> black
         * 
         * black = !white
         * 
         * player_turn = (n & 1) * !player_turn + (n & 0) * player_turn
         */
    } typedef PieceMove;

    


#endif