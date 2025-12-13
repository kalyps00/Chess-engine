#ifndef BOARD_HPP
#define BOARD_HPP
#include <string>
#include "Types.hpp"
class Board
{
private:
    // Bitboard representation empty on 0, pieces 1-12
    long long bitboards[13];
    int board_arr[64];

    // Additional bitboards
    long long
        white_pieces, // 1-5
        black_pieces, // 6-12
        all_pieces,
        empty_squares;
    // Game state
    bool white_to_move;

public:
    Board();
    int get_piece_at(int pos);
    void load_starting_position();
    void load_fen_position(std::string fen);
    void make_move();
    void undo_move();
    bool get_current_player();
    void set_bit(int square, int piece);
};
#endif