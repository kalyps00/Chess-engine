#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include "Types.hpp"

// Tablice ataków (prekalkulowane)
extern Bitboard knight_attacks[64];
extern Bitboard pawn_attacks[2][64];
extern Bitboard king_attacks[64];
extern Bitboard between[64][64];

// Inicjalizacja wszystkich tablic
void init_attacks();

// Generowanie ataków dla figur przesuwnych (w locie)
Bitboard get_rook_attacks(int square, Bitboard occupied);
Bitboard get_bishop_attacks(int square, Bitboard occupied);
Bitboard get_queen_attacks(int square, Bitboard occupied);

#endif
