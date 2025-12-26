#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <cstdint>

extern uint64_t piece_keys[13][64];
extern uint64_t castling_keys[16];
extern uint64_t enpassant_keys[8];
extern uint64_t side_key;

void init_zobrist();

#endif
