#include "Board.hpp"
#include "MoveGenerator.hpp"
#include <sstream>

Bitboard Board::knight_attacks[64];
Bitboard Board::pawn_attacks[2][64];
Bitboard Board::king_attacks[64];
Bitboard Board::between[64][64];

Board::Board()
{
    static bool initialized = false;
    if (!initialized)
    {
        init_knight_attacks();
        init_pawn_attacks();
        init_king_attacks();
        init_between();

        initialized = true;
    }
    for (int i = 0; i < 13; i++)
        bitboards[i] = 0ULL;
    for (int i = 0; i < 64; i++)
        board_arr[i] = EMPTY;

    load_fen_position(starting_fen);
}

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

void Board::init_king_attacks()

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
void Board::init_between()
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
Bitboard Board::get_rook_attacks(int square, Bitboard occupied)
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
Bitboard Board::get_bishop_attacks(int square, Bitboard occupied)
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
Bitboard Board::get_queen_attacks(int square, Bitboard occupied)
{
    return get_rook_attacks(square, occupied) | get_bishop_attacks(square, occupied);
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
    Bitboard mask = 1ULL << square;
    bitboards[piece] |= mask;

    // Update board array
    board_arr[square] = piece;
}
void Board::load_fen_position(std::string fen)
{
    history.clear();
    for (int i = 0; i < 13; i++)
        bitboards[i] = 0ULL;
    for (int i = 0; i < 64; i++)
        board_arr[i] = EMPTY;

    std::stringstream ss(fen);
    std::string board_str, turn_str, castle_str, ep_str;
    int half_clock = 0, full_clock = 1;

    ss >> board_str >> turn_str >> castle_str >> ep_str >> half_clock >> full_clock;

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
                set_bit(rank * 8 + file, piece);
                file++;
            }
        }
    }

    white_to_move = (turn_str == "w");

    castling_rights = 0;
    if (castle_str != "-")
    {
        if (castle_str.find('K') != std::string::npos)
            castling_rights |= 1;
        if (castle_str.find('Q') != std::string::npos)
            castling_rights |= 2;
        if (castle_str.find('k') != std::string::npos)
            castling_rights |= 4;
        if (castle_str.find('q') != std::string::npos)
            castling_rights |= 8;
    }

    enpassant_square = 0ULL;
    if (ep_str != "-" && ep_str.length() >= 2)
    {
        int f = ep_str[0] - 'a';
        int r = ep_str[1] - '1';
        if (f >= 0 && f < 8 && r >= 0 && r < 8)
            enpassant_square = (1ULL << (r * 8 + f));
    }

    halfmove_clock = half_clock;
    fullmove_clock = full_clock;

    update_bitboards();
    update_game_state();
}
void Board::make_move(const Move &move)
{
    history.push_back({castling_rights, enpassant_square, halfmove_clock});
    int source = move.source;
    int destination = move.target;
    int piece = board_arr[source];
    int captured = move.captured;

    if (piece == EMPTY)
        return;
    // remove castling rights if king or rook moves
    if (piece == WHITE_KING)
    {
        castling_rights &= ~0b0011;
    }
    else if (piece == BLACK_KING)
    {
        castling_rights &= ~0b1100;
    }
    else if (piece == WHITE_ROOK)
    {
        if (source == h1)
            castling_rights &= ~0b0001;
        else if (source == a1)
            castling_rights &= ~0b0010;
    }
    else if (piece == BLACK_ROOK)
    {
        if (source == h8)
            castling_rights &= ~0b0100;
        else if (source == a8)
            castling_rights &= ~0b1000;
    }
    // Remove piece from source
    bitboards[piece] &= ~(1ULL << source);
    board_arr[source] = EMPTY;

    if (captured != EMPTY && move.enpassant == 0)
    {
        // Remove captured piece
        bitboards[captured] &= ~(1ULL << destination);

        // Update castling rights if a rook is captured
        if (captured == WHITE_ROOK)
        {
            if (destination == h1)
                castling_rights &= ~0b0001;
            else if (destination == a1)
                castling_rights &= ~0b0010;
        }
        else if (captured == BLACK_ROOK)
        {
            if (destination == h8)
                castling_rights &= ~0b0100;
            else if (destination == a8)
                castling_rights &= ~0b1000;
        }
    }
    else if (move.castle != 0)
    {
        // king side
        if (move.castle == 1)
        {
            int rook_src = (piece == WHITE_KING) ? h1 : h8;
            int rook_dst = (piece == WHITE_KING) ? f1 : f8;
            int rook_piece = (piece == WHITE_KING) ? WHITE_ROOK : BLACK_ROOK;

            // Move rook
            bitboards[rook_piece] &= ~(1ULL << rook_src);
            bitboards[rook_piece] |= (1ULL << rook_dst);
            board_arr[rook_src] = EMPTY;
            board_arr[rook_dst] = rook_piece;
        }
        // queen side
        else if (move.castle == 2)
        {
            int rook_src = (piece == WHITE_KING) ? a1 : a8;
            int rook_dst = (piece == WHITE_KING) ? d1 : d8;
            int rook_piece = (piece == WHITE_KING) ? WHITE_ROOK : BLACK_ROOK;

            // Move rook
            bitboards[rook_piece] &= ~(1ULL << rook_src);
            bitboards[rook_piece] |= (1ULL << rook_dst);
            board_arr[rook_src] = EMPTY;
            board_arr[rook_dst] = rook_piece;
        }
    }

    // Place piece at destination
    int pieceToPlace = (move.promotion != 0) ? move.promotion : piece;
    bitboards[pieceToPlace] |= (1ULL << destination);
    board_arr[destination] = pieceToPlace;
    // update  clock moves
    halfmove_clock = move.captured != 0 ? 0 : halfmove_clock + 1;
    if (white_to_move == false)
    {
        fullmove_clock++;
    }

    // ennpassant
    Bitboard old_enpassant_mask = enpassant_square;
    enpassant_square = 0ULL;

    if (piece == WHITE_PAWN || piece == BLACK_PAWN)
    {
        // Set enpassant square if double pawn move
        if (abs(destination - source) == 16)
        {
            int ep_square = (source + destination) / 2;
            enpassant_square = (1ULL << ep_square);
        }
        // Handle enpassant capture
        else if (old_enpassant_mask && (1ULL << destination) == old_enpassant_mask)
        {
            int captured_pawn_sq = (piece == WHITE_PAWN) ? destination - 8 : destination + 8;
            bitboards[captured] &= ~(1ULL << captured_pawn_sq);
            board_arr[captured_pawn_sq] = EMPTY;
        }
        // update here for perforamnce
        halfmove_clock = 0;
    }

    white_to_move = !white_to_move;
    update_bitboards();
    update_game_state();
}
void Board::undo_move(const Move &move)
{

    if (history.empty())
    {
        return;
    }
    int source = move.source;
    int destination = move.target;
    int piece = board_arr[destination];

    GameState last_state = history.back();
    history.pop_back();
    castling_rights = last_state.castling_rights;
    enpassant_square = last_state.enpassant_square;
    halfmove_clock = last_state.halfmove_clock;
    white_to_move = !white_to_move;
    if (!white_to_move)
    {
        fullmove_clock--;
    }

    // remove piece
    if (move.promotion)
    {
        piece = white_to_move ? WHITE_PAWN : BLACK_PAWN;
        bitboards[move.promotion] &= ~(1ULL << destination);
    }
    else
    {
        bitboards[piece] &= ~(1ULL << destination);
    }
    board_arr[destination] = EMPTY;
    // place piece at source square
    bitboards[piece] |= (1ULL << source);
    board_arr[source] = piece;

    // place captured piece
    if (move.captured != EMPTY && !move.enpassant)
    {
        bitboards[move.captured] |= (1ULL << destination);
        board_arr[destination] = move.captured;
    }
    // enpasant
    if (move.enpassant)
    {
        int captured_sq = (white_to_move ? destination - 8 : destination + 8);
        bitboards[move.captured] |= (1ULL << captured_sq);
        board_arr[captured_sq] = move.captured;
    }
    // casting
    if (move.castle)
    {
        int rook_dst, rook_src, rook_piece = white_to_move ? WHITE_ROOK : BLACK_ROOK;

        if (move.castle == 1)
        {
            rook_dst = (white_to_move ? f1 : f8);
            rook_src = (white_to_move ? h1 : h8);
        }
        else
        {
            rook_dst = (white_to_move ? d1 : d8);
            rook_src = (white_to_move ? a1 : a8);
        }
        // remove rook king was moved in upper part
        bitboards[rook_piece] &= ~(1ULL << rook_dst);
        board_arr[rook_dst] = EMPTY;
        // place rook
        bitboards[rook_piece] |= (1ULL << rook_src);
        board_arr[rook_src] = rook_piece;
    }
    update_bitboards();
    update_game_state();
}
bool Board::is_square_attacked(int square, bool by_white)
{
    // Check pawn attacks
    if (by_white)
    {
        // To check if 'square' is attacked by a WHITE pawn (which attacks upwards),
        // we check if there is a WHITE pawn on the squares that a BLACK pawn at 'square' would attack (downwards).
        if (pawn_attacks[1][square] & bitboards[WHITE_PAWN])
            return true;
    }
    else
    {
        // To check if 'square' is attacked by a BLACK pawn (which attacks downwards),
        // we check if there is a BLACK pawn on the squares that a WHITE pawn at 'square' would attack (upwards).
        if (pawn_attacks[0][square] & bitboards[BLACK_PAWN])
            return true;
    }

    // Check knight attacks
    if (knight_attacks[square] & bitboards[by_white ? WHITE_KNIGHT : BLACK_KNIGHT])
        return true;

    // Check king attacks
    if (king_attacks[square] & bitboards[by_white ? WHITE_KING : BLACK_KING])
        return true;

    // Check bishop/queen attacks (diagonal)
    Bitboard bishops_queens = bitboards[by_white ? WHITE_BISHOP : BLACK_BISHOP] | bitboards[by_white ? WHITE_QUEEN : BLACK_QUEEN];
    if (get_bishop_attacks(square, all_pieces) & bishops_queens)
        return true;

    // Check rook/queen attacks (straight)
    Bitboard rooks_queens = bitboards[by_white ? WHITE_ROOK : BLACK_ROOK] | bitboards[by_white ? WHITE_QUEEN : BLACK_QUEEN];
    if (get_rook_attacks(square, all_pieces) & rooks_queens)
        return true;

    return false;
}
bool Board::is_in_check(bool white_to_move)
{
    int king_square = __builtin_ctzll(bitboards[white_to_move ? WHITE_KING : BLACK_KING]);
    return is_square_attacked(king_square, !white_to_move);
}
// tbi update only changed squares
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
Bitboard Board::get_attackers(int square, bool white_attacker)
{
    Bitboard attackers = 0ULL;
    // Pawns using black lookup table
    attackers |= (pawn_attacks[white_attacker ? 1 : 0][square] & bitboards[white_attacker ? WHITE_PAWN : BLACK_PAWN]);

    // Knights
    attackers |= (knight_attacks[square] & bitboards[white_attacker ? WHITE_KNIGHT : BLACK_KNIGHT]);

    // King
    attackers |= (king_attacks[square] & bitboards[white_attacker ? WHITE_KING : BLACK_KING]);

    // Sliding pieces (Bishops/Queens)
    Bitboard bishops_queens = bitboards[white_attacker ? WHITE_BISHOP : BLACK_BISHOP] |
                              bitboards[white_attacker ? WHITE_QUEEN : BLACK_QUEEN];
    if (bishops_queens)
    {
        attackers |= (get_bishop_attacks(square, all_pieces) & bishops_queens);
    }

    // Sliding pieces (Rooks/Queens)
    Bitboard rooks_queens = bitboards[white_attacker ? WHITE_ROOK : BLACK_ROOK] |
                            bitboards[white_attacker ? WHITE_QUEEN : BLACK_QUEEN];
    if (rooks_queens)
    {
        attackers |= (get_rook_attacks(square, all_pieces) & rooks_queens);
    }

    return attackers;
}

Bitboard Board::get_check_mask(bool white_to_move)
{
    int king_sq = __builtin_ctzll(bitboards[white_to_move ? WHITE_KING : BLACK_KING]);

    Bitboard attackers = get_attackers(king_sq, !white_to_move);

    int count = __builtin_popcountll(attackers);

    // No check -> can move anywhere
    if (count == 0)
        return ~0ULL;

    // Double check -> only king can move
    if (count > 1)
        return 0ULL;

    // Single check -> must capture attacker OR block the line
    int attacker_sq = __builtin_ctzll(attackers);

    // Mask is: squares between king and attacker OR the attacker square itself
    return between[king_sq][attacker_sq] | (1ULL << attacker_sq);
}
// repetetive code
void Board::get_pin_masks(bool white_to_move, Bitboard *pin_masks)
{
    // Default
    for (int i = 0; i < 64; i++)
        pin_masks[i] = ~0ULL;

    int king_sq = __builtin_ctzll(bitboards[white_to_move ? WHITE_KING : BLACK_KING]);
    Bitboard my_pieces = white_to_move ? white_pieces : black_pieces;
    Bitboard op_rooks = bitboards[white_to_move ? BLACK_ROOK : WHITE_ROOK] | bitboards[white_to_move ? BLACK_QUEEN : WHITE_QUEEN];
    Bitboard op_bishops = bitboards[white_to_move ? BLACK_BISHOP : WHITE_BISHOP] | bitboards[white_to_move ? BLACK_QUEEN : WHITE_QUEEN];

    Bitboard pinner_candidates = get_rook_attacks(king_sq, 0) & op_rooks;

    while (pinner_candidates)
    {
        int pinner_sq = __builtin_ctzll(pinner_candidates);
        Bitboard path = between[king_sq][pinner_sq];
        Bitboard pinned = path & my_pieces;

        if (__builtin_popcountll(pinned) == 1)
        {
            int pinned_sq = __builtin_ctzll(pinned);
            // Movement mask for this piece is the attack line (including pinner)
            pin_masks[pinned_sq] = path | (1ULL << pinner_sq);
        }
        pinner_candidates &= pinner_candidates - 1;
    }

    pinner_candidates = get_bishop_attacks(king_sq, 0) & op_bishops;

    while (pinner_candidates)
    {
        int pinner_sq = __builtin_ctzll(pinner_candidates);
        Bitboard path = between[king_sq][pinner_sq];
        Bitboard pinned = path & my_pieces;

        if (__builtin_popcountll(pinned) == 1)
        {
            int pinned_sq = __builtin_ctzll(pinned);
            pin_masks[pinned_sq] = path | (1ULL << pinner_sq);
        }
        pinner_candidates &= pinner_candidates - 1;
    }
}

bool Board::has_insufficient_material()
{
    // If there are any pawns, rooks, or queens, it's not insufficient material
    if (bitboards[WHITE_PAWN] | bitboards[BLACK_PAWN] |
        bitboards[WHITE_ROOK] | bitboards[BLACK_ROOK] |
        bitboards[WHITE_QUEEN] | bitboards[BLACK_QUEEN])
        return false;

    // Count minor pieces
    int white_knights = __builtin_popcountll(bitboards[WHITE_KNIGHT]);
    int white_bishops = __builtin_popcountll(bitboards[WHITE_BISHOP]);
    int black_knights = __builtin_popcountll(bitboards[BLACK_KNIGHT]);
    int black_bishops = __builtin_popcountll(bitboards[BLACK_BISHOP]);

    int white_minors = white_knights + white_bishops;
    int black_minors = black_knights + black_bishops;
    int total_minors = white_minors + black_minors;

    // K vs K
    if (total_minors == 0)
        return true;

    // K+Minor vs K or K vs K+Minor
    if (total_minors == 1)
        return true;

    return false;
}

void Board::update_game_state()
{
    current_legal_moves = MoveGenerator::generate_moves(*this);

    // 1. 50-move rule
    if (halfmove_clock >= 100)
    {
        current_game_status = DRAW;
        return;
    }

    // 2. Insufficient material
    if (has_insufficient_material())
    {
        current_game_status = DRAW;
        return;
    }

    // 3. Checkmate / Stalemate
    if (current_legal_moves.empty())
    {
        if (is_in_check(white_to_move))
        {
            current_game_status = white_to_move ? BLACK_WON : WHITE_WON;
        }
        else
        {
            current_game_status = DRAW;
        }
    }
    else
    {
        current_game_status = ONGOING;
    }
}
