#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using Bitboard = std::uint64_t;

static int popcount(Bitboard b) { return __builtin_popcountll(b); }
static int lsb(Bitboard b) { return __builtin_ctzll(b); }

static Bitboard set_bit(int sq) { return 1ULL << sq; }
static int file_of(int sq) { return sq & 7; }
static int rank_of(int sq) { return sq >> 3; }

// Generate rook mask excluding edge squares so occupancy bits represent blockers.
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

// Generate bishop mask excluding board edges.
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

static Bitboard rook_attack_on_the_fly(int sq, Bitboard occ)
{
    Bitboard attacks = 0ULL;
    int r = rank_of(sq), f = file_of(sq);
    for (int r2 = r + 1; r2 <= 7; ++r2)
    {
        attacks |= set_bit(r2 * 8 + f);
        if (occ & set_bit(r2 * 8 + f))
            break;
    }
    for (int r2 = r - 1; r2 >= 0; --r2)
    {
        attacks |= set_bit(r2 * 8 + f);
        if (occ & set_bit(r2 * 8 + f))
            break;
    }
    for (int f2 = f + 1; f2 <= 7; ++f2)
    {
        attacks |= set_bit(r * 8 + f2);
        if (occ & set_bit(r * 8 + f2))
            break;
    }
    for (int f2 = f - 1; f2 >= 0; --f2)
    {
        attacks |= set_bit(r * 8 + f2);
        if (occ & set_bit(r * 8 + f2))
            break;
    }
    return attacks;
}

static Bitboard bishop_attack_on_the_fly(int sq, Bitboard occ)
{
    Bitboard attacks = 0ULL;
    int r = rank_of(sq), f = file_of(sq);
    for (int r2 = r + 1, f2 = f + 1; r2 <= 7 && f2 <= 7; ++r2, ++f2)
    {
        attacks |= set_bit(r2 * 8 + f2);
        if (occ & set_bit(r2 * 8 + f2))
            break;
    }
    for (int r2 = r + 1, f2 = f - 1; r2 <= 7 && f2 >= 0; ++r2, --f2)
    {
        attacks |= set_bit(r2 * 8 + f2);
        if (occ & set_bit(r2 * 8 + f2))
            break;
    }
    for (int r2 = r - 1, f2 = f + 1; r2 >= 0 && f2 <= 7; --r2, ++f2)
    {
        attacks |= set_bit(r2 * 8 + f2);
        if (occ & set_bit(r2 * 8 + f2))
            break;
    }
    for (int r2 = r - 1, f2 = f - 1; r2 >= 0 && f2 >= 0; --r2, --f2)
    {
        attacks |= set_bit(r2 * 8 + f2);
        if (occ & set_bit(r2 * 8 + f2))
            break;
    }
    return attacks;
}

static std::vector<Bitboard> enumerate_occupancies(Bitboard mask)
{
    std::vector<Bitboard> occupancies;
    Bitboard subset = 0ULL;
    do
    {
        occupancies.push_back(subset);
        subset = (subset - mask) & mask; // iterate all subsets
    } while (subset);
    return occupancies;
}

struct MagicData
{
    Bitboard magic;
    int shift;
    int offset;
};

static Bitboard random_magic(std::mt19937_64 &rng)
{
    // Favor sparse high bits to reduce collisions.
    return rng() & rng() & rng();
}

// Finds magic number for given square and occupancy->attack map.
static MagicData find_magic(int sq, bool rook, const std::vector<Bitboard> &occupancies, const std::vector<Bitboard> &attacks, std::mt19937_64 &rng)
{
    int relevant_bits = popcount(rook ? rook_mask(sq) : bishop_mask(sq));
    int table_size = 1 << relevant_bits;
    std::vector<Bitboard> table(table_size);

    while (true)
    {
        Bitboard magic = random_magic(rng);
        std::fill(table.begin(), table.end(), 0ULL);
        bool fail = false;
        for (std::size_t i = 0; i < occupancies.size(); ++i)
        {
            Bitboard idx = (occupancies[i] * magic) >> (64 - relevant_bits);
            if (table[idx] == 0ULL)
            {
                table[idx] = attacks[i];
            }
            else if (table[idx] != attacks[i])
            {
                fail = true;
                break;
            }
        }
        if (!fail)
            return {magic, 64 - relevant_bits, 0};
    }
}

int main()
{
    std::mt19937_64 rng(0xC0FFEEULL);

    std::vector<MagicData> rook_data(64), bishop_data(64);
    std::vector<int> rook_bits(64), bishop_bits(64);

    // Plain magic tables: each square has its own table [64][1<<bits]
    std::vector<std::vector<Bitboard>> rook_tables(64), bishop_tables(64);

    for (int sq = 0; sq < 64; ++sq)
    {
        Bitboard mask = rook_mask(sq);
        auto occs = enumerate_occupancies(mask);
        std::vector<Bitboard> attacks(occs.size());
        for (std::size_t i = 0; i < occs.size(); ++i)
            attacks[i] = rook_attack_on_the_fly(sq, occs[i]);
        rook_bits[sq] = popcount(mask);
        auto m = find_magic(sq, true, occs, attacks, rng);
        rook_data[sq] = m;

        // Build plain table for this square
        int table_size = 1 << rook_bits[sq];
        rook_tables[sq].resize(table_size, 0ULL);
        for (std::size_t i = 0; i < occs.size(); ++i)
        {
            Bitboard idx = (occs[i] * m.magic) >> m.shift;
            rook_tables[sq][idx] = attacks[i];
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        Bitboard mask = bishop_mask(sq);
        auto occs = enumerate_occupancies(mask);
        std::vector<Bitboard> attacks(occs.size());
        for (std::size_t i = 0; i < occs.size(); ++i)
            attacks[i] = bishop_attack_on_the_fly(sq, occs[i]);
        bishop_bits[sq] = popcount(mask);
        auto m = find_magic(sq, false, occs, attacks, rng);
        bishop_data[sq] = m;

        // Build plain table for this square
        int table_size = 1 << bishop_bits[sq];
        bishop_tables[sq].resize(table_size, 0ULL);
        for (std::size_t i = 0; i < occs.size(); ++i)
        {
            Bitboard idx = (occs[i] * m.magic) >> m.shift;
            bishop_tables[sq][idx] = attacks[i];
        }
    }

    auto print_int_array = [](const char *name, const std::vector<int> &arr)
    {
        std::cout << "const int " << name << "[64] = {\n    ";
        for (int i = 0; i < 64; ++i)
        {
            std::cout << arr[i];
            if (i != 63)
                std::cout << ", ";
            if ((i + 1) % 8 == 0)
                std::cout << "\n    ";
        }
        std::cout << "};\n\n";
    };

    auto print_2d_table = [](const char *name, const std::vector<std::vector<Bitboard>> &tables, int max_size)
    {
        std::cout << "const Bitboard " << name << "[64][" << max_size << "] = {\n";
        for (int sq = 0; sq < 64; ++sq)
        {
            std::cout << "    {";
            for (int i = 0; i < max_size; ++i)
            {
                if (i % 4 == 0 && i > 0)
                    std::cout << "\n     ";
                if (i < static_cast<int>(tables[sq].size()))
                    std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << tables[sq][i] << std::dec;
                else
                    std::cout << "0x0000000000000000";
                if (i + 1 != max_size)
                    std::cout << ",";
            }
            std::cout << "}";
            if (sq != 63)
                std::cout << ",";
            std::cout << "\n";
        }
        std::cout << "};\n\n";
    };

    // Print shifts (64 - relevant bits)
    std::vector<int> rook_shifts(64), bishop_shifts(64);
    for (int i = 0; i < 64; ++i)
    {
        rook_shifts[i] = rook_data[i].shift;
        bishop_shifts[i] = bishop_data[i].shift;
    }

    std::cout << "// Generated by tools/magic_generator.cpp (plain magics)\n";
    std::cout << "namespace magics\n{\n\n";

    print_int_array("rook_shifts", rook_shifts);
    print_int_array("bishop_shifts", bishop_shifts);

    std::vector<Bitboard> rook_magics(64), bishop_magics(64);
    for (int i = 0; i < 64; ++i)
    {
        rook_magics[i] = rook_data[i].magic;
        bishop_magics[i] = bishop_data[i].magic;
    }

    std::cout << "const Bitboard rook_magics[64] = {\n    ";
    for (int i = 0; i < 64; ++i)
    {
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << rook_magics[i] << std::dec;
        if (i != 63)
            std::cout << ",";
        if ((i + 1) % 4 == 0)
            std::cout << "\n    ";
    }
    std::cout << "\n};\n\n";

    std::cout << "const Bitboard bishop_magics[64] = {\n    ";
    for (int i = 0; i < 64; ++i)
    {
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << bishop_magics[i] << std::dec;
        if (i != 63)
            std::cout << ",";
        if ((i + 1) % 4 == 0)
            std::cout << "\n    ";
    }
    std::cout << "\n};\n\n";

    print_2d_table("rook_attacks", rook_tables, 4096);
    print_2d_table("bishop_attacks", bishop_tables, 512);

    std::cout << "} // namespace magics\n";

    long long rook_total = 0, bishop_total = 0;
    for (int i = 0; i < 64; ++i)
    {
        rook_total += rook_tables[i].size();
        bishop_total += bishop_tables[i].size();
    }
    std::cerr << "Rook table size: " << rook_total * sizeof(Bitboard) / 1024.0 << " KB\n";
    std::cerr << "Bishop table size: " << bishop_total * sizeof(Bitboard) / 1024.0 << " KB\n";
}
