#ifndef BOARD_HPP
#define BOARD_HPP
#include <string>
#include "Types.hpp"

class MoveGenerator;

class Board
{
    friend class MoveGenerator;

private:
    // Bitboard representation empty on 0, pieces 1-12
    long long bitboards[13];
    int board_arr[64];

    // Precomputed attack tables
    static long long knight_attacks[64], pawn_attacks[2][64], king_attacks[64];
    // Initialization functions
    static void init_knight_attacks();
    static void init_pawn_attacks();
    static void init_king_attacks();
    // Sliding piece attack generation
    long long get_rook_attacks(int square, long long occupied);
    long long get_bishop_attacks(int square, long long occupied);
    long long get_queen_attacks(int square, long long occupied);
    // Additional bitboards
    long long
        white_pieces, // 1-5
        black_pieces, // 6-12
        all_pieces,
        empty_squares,
        enpassant_square;
    void update_bitboards();
    // Game state
    bool white_to_move;

public:
    Board();
    int get_piece_at(int pos);
    void load_starting_position();
    // void load_fen_position(std::string fen);
    void make_move(const Move &move);
    void set_bit(int square, int piece);
    bool is_white_to_move() const { return white_to_move; }
};
#endif