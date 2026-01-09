#include <cstdint>
#include <iostream>
#include <iomanip>

using Bitboard = std::uint64_t;

static Bitboard set_bit(int sq) { return 1ULL << sq; }
static int file_of(int sq) { return sq & 7; }
static int rank_of(int sq) { return sq >> 3; }

static Bitboard rook_mask(int sq)
{
    Bitboard mask = 0ULL;
    int r = rank_of(sq), f = file_of(sq);
    for (int r2 = r + 1; r2 <= 6; ++r2)
        mask |= set_bit(r2 * 8 + f);
    for (int r2 = r - 1; r2 >= 1; --r2)
        mask |= set_bit(r2 * 8 + f);
    for (int f2 = f + 1; f2 <= 6; ++f2)
        mask |= set_bit(r * 8 + f2);
    for (int f2 = f - 1; f2 >= 1; --f2)
        mask |= set_bit(r * 8 + f2);
    return mask;
}

static Bitboard bishop_mask(int sq)
{
    Bitboard mask = 0ULL;
    int r = rank_of(sq), f = file_of(sq);
    for (int r2 = r + 1, f2 = f + 1; r2 <= 6 && f2 <= 6; ++r2, ++f2)
        mask |= set_bit(r2 * 8 + f2);
    for (int r2 = r + 1, f2 = f - 1; r2 <= 6 && f2 >= 1; ++r2, --f2)
        mask |= set_bit(r2 * 8 + f2);
    for (int r2 = r - 1, f2 = f + 1; r2 >= 1 && f2 <= 6; --r2, ++f2)
        mask |= set_bit(r2 * 8 + f2);
    for (int r2 = r - 1, f2 = f - 1; r2 >= 1 && f2 >= 1; --r2, --f2)
        mask |= set_bit(r2 * 8 + f2);
    return mask;
}

int main()
{
    std::cout << "const Bitboard rook_masks[64] = {\n    ";
    for (int sq = 0; sq < 64; ++sq)
    {
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << rook_mask(sq) << std::dec;
        if (sq != 63)
            std::cout << ",";
        if ((sq + 1) % 4 == 0)
            std::cout << "\n    ";
    }
    std::cout << "\n};\n\n";

    std::cout << "const Bitboard bishop_masks[64] = {\n    ";
    for (int sq = 0; sq < 64; ++sq)
    {
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << bishop_mask(sq) << std::dec;
        if (sq != 63)
            std::cout << ",";
        if ((sq + 1) % 4 == 0)
            std::cout << "\n    ";
    }
    std::cout << "\n};\n";
}
