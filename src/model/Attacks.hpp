#ifndef ATTACKS_HPP
#define ATTACKS_HPP
#include "Types.hpp"
#include "Magics.hpp"

extern Bitboard knight_attacks[64];
extern Bitboard pawn_attacks[2][64];
extern Bitboard king_attacks[64];
extern Bitboard between[64][64];

void init_attacks();

// Inline magic bitboard functions
inline Bitboard get_rook_attacks(int square, Bitboard occupied)
{
    occupied &= magics::rook_masks[square];
    occupied *= magics::rook_magics[square];
    occupied >>= magics::rook_shifts[square];
    return magics::rook_attacks[square][occupied];
}

inline Bitboard get_bishop_attacks(int square, Bitboard occupied)
{
    occupied &= magics::bishop_masks[square];
    occupied *= magics::bishop_magics[square];
    occupied >>= magics::bishop_shifts[square];
    return magics::bishop_attacks[square][occupied];
}

inline Bitboard get_queen_attacks(int square, Bitboard occupied)
{
    return get_rook_attacks(square, occupied) | get_bishop_attacks(square, occupied);
}

#endif
