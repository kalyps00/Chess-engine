#include "Attacks.hpp"
#include "Magics.hpp"
#include <cmath>
#include <cstdlib>

Bitboard knight_attacks[64];
Bitboard pawn_attacks[2][64];
Bitboard king_attacks[64];
Bitboard between[64][64];

static void init_knight_attacks();
static void init_pawn_attacks();
static void init_king_attacks();
static void init_between();

void init_attacks()
{
    init_knight_attacks();
    init_pawn_attacks();
    init_king_attacks();
    init_between();
}

Bitboard get_rook_attacks_slow(int square, Bitboard occupied)
{
    int directions[4] = {8, -8, 1, -1}; // Up, Down, Right, Left
    Bitboard attacks = 0ULL;

    for (int dir = 0; dir < 4; dir++)
    {
        int current_square = square;
        while (true)
        {
            int rank = current_square / 8;
            current_square += directions[dir];
            if (current_square < 0 || current_square >= 64)
                break;
            int new_rank = current_square / 8;

            // Ensure we don't wrap around the board horizontally
            if ((directions[dir] == 1 || directions[dir] == -1) && rank != new_rank)
                break;

            attacks |= (1ULL << current_square);

            if (occupied & (1ULL << current_square))
                break;
        }
    }
    return attacks;
}

Bitboard get_bishop_attacks_slow(int square, Bitboard occupied)
{
    int directions[4] = {9, 7, -9, -7}; // Up-Right, Up-Left, Down-Right, Down-Left
    Bitboard attacks = 0ULL;

    for (int dir = 0; dir < 4; dir++)
    {
        int current_square = square;
        while (true)
        {
            int file = current_square % 8;
            current_square += directions[dir];
            if (current_square < 0 || current_square >= 64)
                break;
            int new_file = current_square % 8;

            // Ensure we don't wrap around the board horizontally
            if (abs(new_file - file) != 1)
                break;

            attacks |= (1ULL << current_square);

            if (occupied & (1ULL << current_square))
                break;
        }
    }
    return attacks;
}

Bitboard get_queen_attacks_slow(int square, Bitboard occupied)
{
    return get_rook_attacks(square, occupied) | get_bishop_attacks(square, occupied);
}

static void init_pawn_attacks()
{
    for (int square = 0; square < 64; square++)
    {
        int rank = square / 8;
        int file = square % 8;

        // Capture up-left (file-1, rank+1) -> +7
        if (rank < 7 && file > 0)
            pawn_attacks[0][square] |= (1ULL << (square + 7));
        // Capture up-right (file+1, rank+1) -> +9
        if (rank < 7 && file < 7)
            pawn_attacks[0][square] |= (1ULL << (square + 9));

        // Black pawns (index 1)
        // Capture down-left (file-1, rank-1) -> -9
        if (rank > 0 && file > 0)
            pawn_attacks[1][square] |= (1ULL << (square - 9));
        // Capture down-right (file+1, rank-1) -> -7
        if (rank > 0 && file < 7)
            pawn_attacks[1][square] |= (1ULL << (square - 7));
    }
}

static void init_knight_attacks()
{
    for (int square = 0; square < 64; square++)
    {
        Bitboard attacks = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        // All 8 possible knight moves (dx, dy)
        int jumps[8][2] = {
            {1, 2}, {-1, 2}, {1, -2}, {-1, -2}, {2, 1}, {-2, 1}, {2, -1}, {-2, -1}};

        for (int i = 0; i < 8; i++)
        {
            int target_file = file + jumps[i][0];
            int target_rank = rank + jumps[i][1];

            if (target_rank >= 0 && target_rank < 8 && target_file >= 0 && target_file < 8)
            {
                int target_square = target_rank * 8 + target_file;
                attacks |= (1ULL << target_square);
            }
        }
        knight_attacks[square] = attacks;
    }
}

static void init_king_attacks()
{
    for (int square = 0; square < 64; square++)
    {
        Bitboard attacks = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        // All 8 possible king moves (dx, dy)
        int moves[8][2] = {
            {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

        for (int i = 0; i < 8; i++)
        {
            int target_file = file + moves[i][0];
            int target_rank = rank + moves[i][1];

            if (target_rank >= 0 && target_rank < 8 && target_file >= 0 && target_file < 8)
            {
                int target_square = target_rank * 8 + target_file;
                attacks |= (1ULL << target_square);
            }
        }
        king_attacks[square] = attacks;
    }
}

static void init_between()
{
    for (int sq1 = 0; sq1 < 64; sq1++)
    {
        for (int sq2 = 0; sq2 < 64; sq2++)
        {
            between[sq1][sq2] = 0ULL;
            if (sq1 == sq2)
            {
                continue;
            }
            int file1 = sq1 % 8, file2 = sq2 % 8;
            int rank1 = sq1 / 8, rank2 = sq2 / 8;
            int dx = file2 - file1, dy = rank2 - rank1;
            // same file, rank, \ and /
            if (dx != 0 && dy != 0 && abs(dx) != abs(dy))
            {
                continue;
            }
            int step_x = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
            int step_y = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
            int step = step_y * 8 + step_x;
            int current = sq1 + step;
            while (current != sq2 && current >= 0 && current < 64)
            {
                between[sq1][sq2] |= (1ULL << current);
                current += step;
            }
        }
    }
}
