#ifndef MOVE_GENERATOR_HPP
#define MOVE_GENERATOR_HPP

#include "Board.hpp"
#include "Types.hpp"
#include <vector>

class MoveGenerator
{
public:
    static std::vector<Move> generate_moves(Board &board);

private:
    static void generate_pawn_moves(Board &board, std::vector<Move> &moves, Bitboard check_mask, Bitboard *pin_masks);
    static void generate_knight_moves(Board &board, std::vector<Move> &moves, Bitboard check_mask, Bitboard *pin_masks);
    static void generate_sliding_moves(Board &board, std::vector<Move> &moves, Bitboard check_mask, Bitboard *pin_masks);
    static void generate_king_moves(Board &board, std::vector<Move> &moves);
};

#endif
