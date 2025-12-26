#include "Zobrist.hpp"

uint64_t piece_keys[13][64];
uint64_t castling_keys[16];
uint64_t enpassant_keys[8];
uint64_t side_key;

static uint64_t seed = 694202137ULL;

// Pseudo-random number generator (SplitMix64)
uint64_t get_random_number()
{
    uint64_t z = (seed += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

void init_zobrist()
{
    for (int i = 1; i < 13; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            piece_keys[i][j] = get_random_number();
        }
    }
    for (int i = 0; i < 16; i++)
    {
        castling_keys[i] = get_random_number();
    }
    for (int i = 0; i < 8; i++)
    {
        enpassant_keys[i] = get_random_number();
    }
    side_key = get_random_number();
}
uint64_t hash_position(Board &board)
{
    uint64_t key = 0ULL;

    // Pieces
    for (int square = 0; square < 64; square++)
    {
        int piece = board.board_arr[square];
        if (piece != EMPTY)
        {
            key ^= piece_keys[piece][square];
        }
    }

    // Castling rights
    key ^= castling_keys[board.castling_rights];

    // En passant
    if (board.enpassant_square)
    {
        int file = __builtin_ctzll(board.enpassant_square) % 8;
        key ^= enpassant_keys[file];
    }

    // Side to move
    if (!board.white_to_move)
    {
        key ^= side_key;
    }

    return key;
}
