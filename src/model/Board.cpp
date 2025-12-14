#include "Board.hpp"

long long Board::knight_attacks[64];
long long Board::pawn_attacks[2][64];
long long Board::king_attacks[64];

void Board::init_pawn_attacks()
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

void Board::init_knight_attacks()
{
    for (int square = 0; square < 64; square++)
    {
        long long attacks = 0ULL;
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

void Board::init_king_attacks()

{
    for (int square = 0; square < 64; square++)
    {
        long long attacks = 0ULL;
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
long long Board::get_rook_attacks(int square, long long occupied)
{
    int directions[4] = {8, -8, 1, -1}; // Up, Down, Right, Left
    long long attacks = 0ULL;

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
long long Board::get_bishop_attacks(int square, long long occupied)
{
    int directions[4] = {9, 7, -9, -7}; // Up-Right, Up-Left, Down-Right, Down-Left
    long long attacks = 0ULL;

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
long long Board::get_queen_attacks(int square, long long occupied)
{
    return get_rook_attacks(square, occupied) | get_bishop_attacks(square, occupied);
}
Board::Board()
{
    static bool initialized = false;
    if (!initialized)
    {
        init_knight_attacks();
        init_pawn_attacks();
        init_king_attacks();
        initialized = true;
    } // Clear bitboards and Board array
    for (int i = 0; i < 13; i++)
        bitboards[i] = 0ULL;
    for (int i = 0; i < 64; i++)
        board_arr[i] = EMPTY;

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
    update_bitboards();
}
void Board::make_move(const Move &move)
{
    int source = move.source;
    int destination = move.target;
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

    int pieceToPlace = (move.promotion != 0) ? move.promotion : piece;

    // Place piece at destination
    bitboards[pieceToPlace] |= (1ULL << destination);
    board_arr[destination] = pieceToPlace;

    white_to_move = !white_to_move;
    update_bitboards();
}
void Board::update_bitboards()
{
    white_pieces = 0ULL;
    black_pieces = 0ULL;
    all_pieces = 0ULL;
    empty_squares = 0ULL;

    for (int i = 1; i <= 6; i++)
    {
        white_pieces |= bitboards[i];
    }
    for (int i = 7; i <= 12; i++)
    {
        black_pieces |= bitboards[i];
    }
    all_pieces = white_pieces | black_pieces;
    empty_squares = ~all_pieces;
}