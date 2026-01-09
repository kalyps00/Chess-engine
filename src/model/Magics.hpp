#ifndef MAGICS_HPP
#define MAGICS_HPP

#include "Types.hpp"
namespace magics
{
    // Shifts and magics for plain magic bitboards. Definitions in Magics.cpp.
    extern const int rook_shifts[64];
    extern const int bishop_shifts[64];

    extern const Bitboard rook_magics[64];
    extern const Bitboard bishop_magics[64];

    // Masks for relevant occupancy bits (excluding edges)
    extern const Bitboard rook_masks[64];
    extern const Bitboard bishop_masks[64];

    // Plain attack tables: rook_attacks[sq][index], bishop_attacks[sq][index]
    extern const Bitboard rook_attacks[64][4096];
    extern const Bitboard bishop_attacks[64][512];
}

#endif
