#include "Board.hpp"
#include "MoveGenerator.hpp"
#include "Zobrist.hpp"
#include "Attacks.hpp"
#include "FenParser.hpp"
#include <sstream>

std::vector<Move> Board::current_legal_moves;

Board::Board()
{
    static bool initialized = false;
    if (!initialized)
    {
        init_attacks();
        init_zobrist();

        initialized = true;
    }
    for (int i = 0; i < 13; i++)
        bitboards[i] = 0ULL;
    for (int i = 0; i < 64; i++)
        board_arr[i] = EMPTY;

    load_fen_position(*this, starting_fen);
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
void Board::make_move(const Move &move, bool update_state)
{
    history.push_back({castling_rights, enpassant_square, halfmove_clock, current_zobrist_key});
    int source = move.source;
    int destination = move.target;
    int piece = board_arr[source];
    int captured = move.captured;

    if (piece == EMPTY)
        return;

    // XOR out old enpassant
    if (enpassant_square)
    {
        int file = __builtin_ctzll(enpassant_square) % 8;
        current_zobrist_key ^= enpassant_keys[file];
    }

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
    Bitboard fromMask = 1ULL << source;
    Bitboard toMask = 1ULL << destination;
    current_zobrist_key ^= piece_keys[piece][source];
    bitboards[piece] &= ~fromMask;
    board_arr[source] = EMPTY;
    // Incremental update: remove from source
    all_pieces ^= fromMask;
    if (white_to_move)
        white_pieces ^= fromMask;
    else
        black_pieces ^= fromMask;

    if (captured != EMPTY && move.enpassant == 0)
    {
        // Remove captured piece
        current_zobrist_key ^= piece_keys[captured][destination];
        bitboards[captured] &= ~toMask;
        // Incremental update: remove captured
        all_pieces ^= toMask;
        if (!white_to_move)
            white_pieces ^= toMask;
        else
            black_pieces ^= toMask;

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
            Bitboard rookFrom = 1ULL << rook_src;
            Bitboard rookTo = 1ULL << rook_dst;
            current_zobrist_key ^= piece_keys[rook_piece][rook_src];
            current_zobrist_key ^= piece_keys[rook_piece][rook_dst];
            bitboards[rook_piece] ^= rookFrom | rookTo;
            board_arr[rook_src] = EMPTY;
            board_arr[rook_dst] = rook_piece;
            // Incremental update: rook move
            all_pieces ^= rookFrom | rookTo;
            if (white_to_move)
                white_pieces ^= rookFrom | rookTo;
            else
                black_pieces ^= rookFrom | rookTo;
        }
        // queen side
        else if (move.castle == 2)
        {
            int rook_src = (piece == WHITE_KING) ? a1 : a8;
            int rook_dst = (piece == WHITE_KING) ? d1 : d8;
            int rook_piece = (piece == WHITE_KING) ? WHITE_ROOK : BLACK_ROOK;

            // Move rook
            Bitboard rookFrom = 1ULL << rook_src;
            Bitboard rookTo = 1ULL << rook_dst;
            current_zobrist_key ^= piece_keys[rook_piece][rook_src];
            current_zobrist_key ^= piece_keys[rook_piece][rook_dst];
            bitboards[rook_piece] ^= rookFrom | rookTo;
            board_arr[rook_src] = EMPTY;
            board_arr[rook_dst] = rook_piece;
            // Incremental update: rook move
            all_pieces ^= rookFrom | rookTo;
            if (white_to_move)
                white_pieces ^= rookFrom | rookTo;
            else
                black_pieces ^= rookFrom | rookTo;
        }
    }

    // Place piece at destination
    int pieceToPlace = (move.promotion != 0) ? move.promotion : piece;
    current_zobrist_key ^= piece_keys[pieceToPlace][destination];
    bitboards[pieceToPlace] |= toMask;
    board_arr[destination] = pieceToPlace;
    // Incremental update: add to destination
    all_pieces |= toMask;
    if (white_to_move)
        white_pieces |= toMask;
    else
        black_pieces |= toMask;
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
            Bitboard epCapMask = 1ULL << captured_pawn_sq;
            current_zobrist_key ^= piece_keys[captured][captured_pawn_sq];
            bitboards[captured] &= ~epCapMask;
            board_arr[captured_pawn_sq] = EMPTY;
            // Incremental update: remove en passant captured pawn
            all_pieces ^= epCapMask;
            if (!white_to_move)
                white_pieces ^= epCapMask;
            else
                black_pieces ^= epCapMask;
        }
        // update here for perforamnce
        halfmove_clock = 0;
    }

    // XOR in new castling rights
    if (castling_rights != history.back().castling_rights)
    {
        current_zobrist_key ^= castling_keys[history.back().castling_rights];
        current_zobrist_key ^= castling_keys[castling_rights];
    }

    // XOR in new enpassant
    if (enpassant_square)
    {
        int file = __builtin_ctzll(enpassant_square) % 8;
        current_zobrist_key ^= enpassant_keys[file];
    }

    // Side to move
    current_zobrist_key ^= side_key;

    white_to_move = !white_to_move;
    if (update_state)
        update_game_state();
}
void Board::undo_move(const Move &move, bool update_state)
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
    current_zobrist_key = last_state.zobrist_position_key;
    white_to_move = !white_to_move;
    if (!white_to_move)
    {
        fullmove_clock--;
    }

    // remove piece
    Bitboard fromMask = 1ULL << source;
    Bitboard toMask = 1ULL << destination;
    if (move.promotion)
    {
        piece = white_to_move ? WHITE_PAWN : BLACK_PAWN;
        bitboards[move.promotion] &= ~toMask;
    }
    else
    {
        bitboards[piece] &= ~toMask;
    }
    board_arr[destination] = EMPTY;
    // Incremental update: remove from destination
    all_pieces ^= toMask;
    if (white_to_move)
        white_pieces ^= toMask;
    else
        black_pieces ^= toMask;

    // place piece at source square
    bitboards[piece] |= fromMask;
    board_arr[source] = piece;
    // Incremental update: add to source
    all_pieces |= fromMask;
    if (white_to_move)
        white_pieces |= fromMask;
    else
        black_pieces |= fromMask;

    // place captured piece
    if (move.captured != EMPTY && !move.enpassant)
    {
        bitboards[move.captured] |= toMask;
        board_arr[destination] = move.captured;
        // Incremental update: restore captured
        all_pieces |= toMask;
        if (!white_to_move)
            white_pieces |= toMask;
        else
            black_pieces |= toMask;
    }
    // enpasant
    if (move.enpassant)
    {
        int captured_sq = (white_to_move ? destination - 8 : destination + 8);
        Bitboard epMask = 1ULL << captured_sq;
        bitboards[move.captured] |= epMask;
        board_arr[captured_sq] = move.captured;
        // Incremental update: restore en passant captured
        all_pieces |= epMask;
        if (!white_to_move)
            white_pieces |= epMask;
        else
            black_pieces |= epMask;
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
        Bitboard rookFrom = 1ULL << rook_src;
        Bitboard rookTo = 1ULL << rook_dst;
        bitboards[rook_piece] ^= rookFrom | rookTo;
        board_arr[rook_dst] = EMPTY;
        board_arr[rook_src] = rook_piece;
        // Incremental update: rook undo
        all_pieces ^= rookFrom | rookTo;
        if (white_to_move)
            white_pieces ^= rookFrom | rookTo;
        else
            black_pieces ^= rookFrom | rookTo;
    }
    if (update_state)
        update_game_state();
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

    for (int i = 1; i <= 6; i++)
    {
        white_pieces |= bitboards[i];
    }
    for (int i = 7; i <= 12; i++)
    {
        black_pieces |= bitboards[i];
    }
    all_pieces = white_pieces | black_pieces;
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
        Bitboard pinned = path & all_pieces;
        // was __builtin_popcountll(pinned) == 1
        if (pinned && !(pinned & (pinned - 1)) && (pinned & my_pieces))
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
        Bitboard pinned = path & all_pieces;

        if (__builtin_popcountll(pinned) == 1 && (pinned & my_pieces))
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

bool Board::is_repetition()
{
    int count = 1;
    int history_size = history.size();
    for (int i = history_size - 2; i >= 0 && i >= history_size - halfmove_clock; i -= 2)
    {
        if (history[i].zobrist_position_key == current_zobrist_key)
        {
            count++;
            if (count >= 3)
                return true;
        }
    }
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

    // 3. Threefold repetition
    if (is_repetition())
    {
        current_game_status = DRAW;
        return;
    }

    // 4. Checkmate / Stalemate
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
void Board::reset_game()
{
    current_game_status = ONGOING;
    load_fen_position(*this, starting_fen);
}