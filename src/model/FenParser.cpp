#include "FenParser.hpp"
#include "Board.hpp"
#include <sstream>

void FenParser::load_fen_position(Board &board, const std::string &fen)
{
    // 1. Resetowanie całej planszy
    board.history.clear();
    for (int i = 0; i < 13; i++)
        board.bitboards[i] = 0ULL;
    for (int i = 0; i < 64; i++)
        board.board_arr[i] = EMPTY;

    // 2. Ekstrakcja danych (strumień sam ogarnia spacje)
    std::stringstream ss(fen);
    std::string board_str, turn_str, castle_str, ep_str;
    int half_clock = 0, full_clock = 1;

    ss >> board_str >> turn_str >> castle_str >> ep_str >> half_clock >> full_clock;

    // 3. Parsowanie układu figur
    int rank = 7, file = 0;
    for (char c : board_str)
    {
        if (c == '/')
        {
            rank--;
            file = 0;
        }
        else if (isdigit(c))
        {
            file += (c - '0');
        }
        else
        {
            int piece = EMPTY;
            // Mapping znak -> figura
            if (c == 'P')
                piece = WHITE_PAWN;
            else if (c == 'N')
                piece = WHITE_KNIGHT;
            else if (c == 'B')
                piece = WHITE_BISHOP;
            else if (c == 'R')
                piece = WHITE_ROOK;
            else if (c == 'Q')
                piece = WHITE_QUEEN;
            else if (c == 'K')
                piece = WHITE_KING;
            else if (c == 'p')
                piece = BLACK_PAWN;
            else if (c == 'n')
                piece = BLACK_KNIGHT;
            else if (c == 'b')
                piece = BLACK_BISHOP;
            else if (c == 'r')
                piece = BLACK_ROOK;
            else if (c == 'q')
                piece = BLACK_QUEEN;
            else if (c == 'k')
                piece = BLACK_KING;

            if (piece != EMPTY)
            {
                board.set_bit(rank * 8 + file, piece);
                file++;
            }
        }
    }

    // 4. Ustawianie stanu gry
    board.white_to_move = (turn_str == "w");

    board.castling_rights = 0;
    if (castle_str != "-")
    {
        if (castle_str.find('K') != std::string::npos)
            board.castling_rights |= 1;
        if (castle_str.find('Q') != std::string::npos)
            board.castling_rights |= 2;
        if (castle_str.find('k') != std::string::npos)
            board.castling_rights |= 4;
        if (castle_str.find('q') != std::string::npos)
            board.castling_rights |= 8;
    }

    board.enpassant_square = 0ULL;
    if (ep_str != "-" && ep_str.length() >= 2)
    {
        int f = ep_str[0] - 'a';
        int r = ep_str[1] - '1';
        if (f >= 0 && f < 8 && r >= 0 && r < 8)
            board.enpassant_square = (1ULL << (r * 8 + f));
    }

    board.halfmove_clock = half_clock;
    board.fullmove_clock = full_clock;

    board.update_bitboards();
    board.update_game_state();
}
