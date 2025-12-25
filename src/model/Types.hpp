#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

// Alias for bitboards - makes code more readable and ensures 64-bit size
using Bitboard = uint64_t;

enum PieceIndex
{
    EMPTY = 0,
    WHITE_PAWN = 1,
    WHITE_KNIGHT = 2,
    WHITE_BISHOP = 3,
    WHITE_ROOK = 4,
    WHITE_QUEEN = 5,
    WHITE_KING = 6,
    BLACK_PAWN = 7,
    BLACK_KNIGHT = 8,
    BLACK_BISHOP = 9,
    BLACK_ROOK = 10,
    BLACK_QUEEN = 11,
    BLACK_KING = 12
};
inline bool
is_white_piece(int piece)
{
    return piece >= WHITE_PAWN && piece <= WHITE_KING;
}

inline bool is_black_piece(int piece)
{
    return piece >= BLACK_PAWN && piece <= BLACK_KING;
}

// clang-format off
    enum Square {
    a1, b1, c1, d1, e1, f1, g1, h1, 
    a2, b2, c2, d2, e2, f2, g2, h2, 
    a3, b3, c3, d3, e3, f3, g3, h3, 
    a4, b4, c4, d4, e4, f4, g4, h4, 
    a5, b5, c5, d5, e5, f5, g5, h5, 
    a6, b6, c6, d6, e6, f6, g6, h6, 
    a7, b7, c7, d7, e7, f7, g7, h7, 
    a8, b8, c8, d8, e8, f8, g8, h8
    // clang-format on
};

struct Move
{
    int source;
    int target;
    int piece;
    int captured;
    int promotion; // 0 if no promotion
    int enpassant; // 0 if not an enpassant move
    int castle;    // 0 if not a castling move 1 if king side, 2 if queen side

    Move() : source(0), target(0), piece(0), captured(0), promotion(0), enpassant(0), castle(0) {}

    // Constructor for standard moves
    Move(int src, int tgt, int p, int cap = 0, int prom = 0, int ep = 0, int cas = 0)
        : source(src), target(tgt), piece(p), captured(cap), promotion(prom), enpassant(ep), castle(cas) {}
};
struct GameState
{
    int castling_rights;
    Bitboard enpassant_square;
    int halfmove_clock;
};
enum Status
{
    ONGOING,
    WHITE_WON,
    BLACK_WON,
    DRAW,
};
#endif