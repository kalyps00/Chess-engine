#ifndef BOARD_HPP
#define BOARD_HPP
#include <string>
#include <Types.hpp>
class Board
{
private:
    // add bitboards array
    //  Engine part - bitboards

    //  white pices bitboards
    long long
        white_pawns,
        white_knights,
        white_bishops,
        white_rooks,
        white_queens,
        white_king;
    // black pices bitboards
    long long
        black_pawns,
        black_knights,
        black_bishops,
        black_rooks,
        black_queens,
        black_king;
    // Additional bitboards
    long long
        white_pieces,
        black_pieces,
        all_pieces,
        empty_squares;
    // Game state
    bool white_to_move;

    // View part
    int board_arr[64];

public:
    Board();
    int get_piece_at(int pos);
    void load_starting_position();
    void load_fen_position(std::string fen);
    void make_move();
    void undo_move();
    bool get_current_player();
};
#endif