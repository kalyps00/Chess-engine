#include "Board.hpp"

Board::Board()
{
    // Clear bitboards and Board array
    for (int i = 0; i < 13; i++)
        bitboards[i] = 0ULL;
    for (int i = 0; i < 64; i++)
        board_arr[i] = EMPTY;

    white_pieces = 0ULL;
    black_pieces = 0ULL;
    all_pieces = 0ULL;
    empty_squares = ~0ULL;

    load_starting_position();
    white_to_move = true;
}

int Board::get_piece_at(int pos)
{
    if (pos < 0 || pos >= 64)
        return 0;
    return board_arr[pos];
}

void Board::set_bit(int square, int piece)
{
    // Update Bitboard
    long long mask = 1ULL << square;
    bitboards[piece] |= mask;

    // Update board array
    board_arr[square] = piece;
}

void Board::load_starting_position()
{
    // White Pieces
    set_bit(a1, WHITE_ROOK);
    set_bit(h1, WHITE_ROOK);
    set_bit(b1, WHITE_KNIGHT);
    set_bit(g1, WHITE_KNIGHT);
    set_bit(c1, WHITE_BISHOP);
    set_bit(f1, WHITE_BISHOP);
    set_bit(d1, WHITE_QUEEN);
    set_bit(e1, WHITE_KING);

    // White Pawns
    for (int i = 8; i < 16; i++)
        set_bit(i, WHITE_PAWN);

    // Black Pieces
    set_bit(a8, BLACK_ROOK);
    set_bit(h8, BLACK_ROOK);
    set_bit(b8, BLACK_KNIGHT);
    set_bit(g8, BLACK_KNIGHT);
    set_bit(c8, BLACK_BISHOP);
    set_bit(f8, BLACK_BISHOP);
    set_bit(d8, BLACK_QUEEN);
    set_bit(e8, BLACK_KING);

    // Black Pawns
    for (int i = 48; i < 56; i++)
        set_bit(i, BLACK_PAWN);
}
void Board::make_move(int source, int destination)
{
    int piece = board_arr[source];
    int captured = board_arr[destination];

    if (piece == EMPTY)
        return;

    long long move_mask = (1ULL << source) | (1ULL << destination);
    bitboards[piece] ^= move_mask;

    if (captured != EMPTY)
    {
        bitboards[captured] ^= (1ULL << destination);
    }
    board_arr[source] = EMPTY;
    board_arr[destination] = piece;

    white_to_move = !white_to_move;
}
