#ifndef BOARD_HPP
#define BOARD_HPP
#include <string>
#include <vector>
#include "Types.hpp"

class MoveGenerator;

class Board
{
    friend class MoveGenerator;

private:
    // Bitboard representation empty on 0, pieces 1-12
    Bitboard bitboards[13];
    int board_arr[64];

    // Precomputed attack tables
    static Bitboard knight_attacks[64], pawn_attacks[2][64], king_attacks[64], between[64][64];
    // Initialization functions
    static void init_knight_attacks();
    static void init_pawn_attacks();
    static void init_king_attacks();
    static void init_between();
    // Sliding piece attack generation
    Bitboard get_rook_attacks(int square, Bitboard occupied);
    Bitboard get_bishop_attacks(int square, Bitboard occupied);
    Bitboard get_queen_attacks(int square, Bitboard occupied);
    // Additional bitboards
    Bitboard
        white_pieces, // 1-5
        black_pieces, // 6-12
        all_pieces,
        empty_squares,
        enpassant_square;
    void update_bitboards();
    int castling_rights; // bit 0: white king-side, bit 1: white queen-side, bit 2: black king-side, bit 3: black queen-side
    bool is_square_attacked(int square, bool by_white);
    bool is_in_check(bool white_to_move);
    // Game state
    bool white_to_move;
    int halfmove_clock = 0;
    int fullmove_clock = 1;
    std::vector<GameState> history;
    std::vector<Move> current_legal_moves;
    Status current_game_status;
    void update_game_state();

public:
    Board();
    int get_piece_at(int pos);
    void load_fen_position(std::string fen);
    std::string export_fen_position(); // tbi
    void make_move(const Move &move);
    void undo_move(const Move &move);
    void set_bit(int square, int piece);
    bool is_white_to_move() const { return white_to_move; }
    Bitboard get_check_mask(bool white_to_move);
    void get_pin_masks(bool white_to_move, Bitboard *pin_masks);
    Bitboard get_attackers(int square, bool white_attacker);
    Status get_game_status() const { return current_game_status; }
    const std::vector<Move> &get_legal_moves() const { return current_legal_moves; }
    bool has_insufficient_material();
    std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
};
#endif