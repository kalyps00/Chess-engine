#include "MoveGenerator.hpp"

std::vector<Move> MoveGenerator::generate_moves(Board &board)
{
    std::vector<Move> moves;
    moves.reserve(256);

    generate_pawn_moves(board, moves);
    generate_knight_moves(board, moves);
    generate_king_moves(board, moves);
    generate_sliding_moves(board, moves);

    return moves;
}
void MoveGenerator::generate_pawn_moves(Board &board, std::vector<Move> &moves)
{
    bool white_to_move = board.is_white_to_move();
    int pawn_type = white_to_move ? WHITE_PAWN : BLACK_PAWN;
    Bitboard pawns_mask = board.bitboards[pawn_type];
    Bitboard enemy_pieces_mask = white_to_move ? board.black_pieces : board.white_pieces;
    int direction = white_to_move ? 8 : -8;
    Bitboard start_mask = white_to_move ? (0xFFULL << 8) : (0xFFULL << 48);
    Bitboard promotion_rank_mask = white_to_move ? (0xFFULL << 56) : (0xFFULL << 0);

    while (pawns_mask)
    {
        int source_square = __builtin_ctzll(pawns_mask);
        int target_square = source_square + direction;
        if ((1ULL << target_square) & board.empty_squares)
        {
            // Promotions
            if ((1ULL << target_square) & promotion_rank_mask)
            {
                moves.emplace_back(source_square, target_square, pawn_type, 0, white_to_move ? WHITE_QUEEN : BLACK_QUEEN);
                moves.emplace_back(source_square, target_square, pawn_type, 0, white_to_move ? WHITE_ROOK : BLACK_ROOK);
                moves.emplace_back(source_square, target_square, pawn_type, 0, white_to_move ? WHITE_BISHOP : BLACK_BISHOP);
                moves.emplace_back(source_square, target_square, pawn_type, 0, white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT);
            }
            else
                moves.emplace_back(source_square, target_square, pawn_type);
            // Double move from starting position
            if ((1ULL << source_square) & start_mask)
            {
                int double_move_square = source_square + 2 * direction;
                if ((1ULL << double_move_square) & board.empty_squares)
                {
                    moves.emplace_back(source_square, double_move_square, pawn_type);
                }
            }
        }
        // Captures
        Bitboard pawn_attacks_mask = Board::pawn_attacks[white_to_move ? 0 : 1][source_square];
        Bitboard capture_targets = pawn_attacks_mask & enemy_pieces_mask;
        while (capture_targets)
        {
            int attack_square = __builtin_ctzll(capture_targets);
            int captured_piece = board.board_arr[attack_square];

            // Promotions on capture
            if ((1ULL << attack_square) & promotion_rank_mask)
            {
                moves.emplace_back(source_square, attack_square, pawn_type, captured_piece, white_to_move ? WHITE_QUEEN : BLACK_QUEEN);
                moves.emplace_back(source_square, attack_square, pawn_type, captured_piece, white_to_move ? WHITE_ROOK : BLACK_ROOK);
                moves.emplace_back(source_square, attack_square, pawn_type, captured_piece, white_to_move ? WHITE_BISHOP : BLACK_BISHOP);
                moves.emplace_back(source_square, attack_square, pawn_type, captured_piece, white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT);
            }
            else
            {
                moves.emplace_back(source_square, attack_square, pawn_type, captured_piece);
            }
            capture_targets &= capture_targets - 1;
        }
        if (board.enpassant_square)
        {
            Bitboard ep_attacks = pawn_attacks_mask & board.enpassant_square;
            if (ep_attacks)
            {
                int ep_square = __builtin_ctzll(ep_attacks);
                int captured_pawn = white_to_move ? BLACK_PAWN : WHITE_PAWN;
                moves.emplace_back(source_square, ep_square, pawn_type, captured_pawn, 0, 1);
            }
        }
        // delete processed pawn
        pawns_mask &= pawns_mask - 1;
    }
}
void MoveGenerator::generate_knight_moves(Board &board, std::vector<Move> &moves)
{
    bool white_to_move = board.white_to_move;
    int piece_type = white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT;
    Bitboard knights_mask = board.bitboards[piece_type];
    Bitboard own_pieces_mask = white_to_move ? board.white_pieces : board.black_pieces;

    while (knights_mask)
    {
        int source_square = __builtin_ctzll(knights_mask);
        Bitboard attacks = Board::knight_attacks[source_square] & ~own_pieces_mask;

        while (attacks)
        {
            int target_square = __builtin_ctzll(attacks);
            int captured_piece = board.board_arr[target_square];
            moves.emplace_back(source_square, target_square, piece_type, captured_piece);
            attacks &= attacks - 1;
        }
        knights_mask &= knights_mask - 1;
    }
}
void MoveGenerator::generate_king_moves(Board &board, std::vector<Move> &moves)
{
    bool white_to_move = board.white_to_move;
    int piece_type = white_to_move ? WHITE_KING : BLACK_KING;
    Bitboard king_mask = board.bitboards[piece_type];
    Bitboard own_pieces_mask = white_to_move ? board.white_pieces : board.black_pieces;

    if (king_mask)
    {
        int source_square = __builtin_ctzll(king_mask);
        Bitboard attacks = Board::king_attacks[source_square] & ~own_pieces_mask;

        while (attacks)
        {
            int target_square = __builtin_ctzll(attacks);
            int captured_piece = board.board_arr[target_square];
            moves.emplace_back(source_square, target_square, piece_type, captured_piece);
            attacks &= attacks - 1;
        }

        // Castling
        if (white_to_move)
        {
            // King Side (e1 -> g1)
            if ((board.castling_rights & 1) &&
                board.board_arr[f1] == EMPTY && board.board_arr[g1] == EMPTY)
            {
                if (!board.is_square_attacked(e1, false) &&
                    !board.is_square_attacked(f1, false) &&
                    !board.is_square_attacked(g1, false))
                {
                    moves.emplace_back(e1, g1, WHITE_KING, 0, 0, 0, 1);
                }
            }
            // Queen Side (e1 -> c1)
            if ((board.castling_rights & 2) &&
                board.board_arr[d1] == EMPTY && board.board_arr[c1] == EMPTY && board.board_arr[b1] == EMPTY)
            {
                if (!board.is_square_attacked(e1, false) &&
                    !board.is_square_attacked(d1, false) &&
                    !board.is_square_attacked(c1, false))
                {
                    moves.emplace_back(e1, c1, WHITE_KING, 0, 0, 0, 2);
                }
            }
        }
        else
        {
            // King Side (e8 -> g8)
            if ((board.castling_rights & 4) &&
                board.board_arr[f8] == EMPTY && board.board_arr[g8] == EMPTY)
            {
                if (!board.is_square_attacked(e8, true) &&
                    !board.is_square_attacked(f8, true) &&
                    !board.is_square_attacked(g8, true))
                {
                    moves.emplace_back(e8, g8, BLACK_KING, 0, 0, 0, 1);
                }
            }
            // Queen Side (e8 -> c8)
            if ((board.castling_rights & 8) &&
                board.board_arr[d8] == EMPTY && board.board_arr[c8] == EMPTY && board.board_arr[b8] == EMPTY)
            {
                if (!board.is_square_attacked(e8, true) &&
                    !board.is_square_attacked(d8, true) &&
                    !board.is_square_attacked(c8, true))
                {
                    moves.emplace_back(e8, c8, BLACK_KING, 0, 0, 0, 2);
                }
            }
        }
    }
}
// tbi magic bitboards for sliding pieces
void MoveGenerator::generate_sliding_moves(Board &board, std::vector<Move> &moves)
{
    bool white_to_move = board.white_to_move;
    Bitboard own_pieces_mask = white_to_move ? board.white_pieces : board.black_pieces;

    int piece_types[3] = {
        white_to_move ? WHITE_BISHOP : BLACK_BISHOP,
        white_to_move ? WHITE_ROOK : BLACK_ROOK,
        white_to_move ? WHITE_QUEEN : BLACK_QUEEN};

    for (int piece_type : piece_types)
    {
        Bitboard pieces_mask = board.bitboards[piece_type];
        while (pieces_mask)
        {
            int source_square = __builtin_ctzll(pieces_mask);
            Bitboard attacks = 0ULL;

            if (piece_type == WHITE_BISHOP || piece_type == BLACK_BISHOP)
                attacks = board.get_bishop_attacks(source_square, board.all_pieces);
            else if (piece_type == WHITE_ROOK || piece_type == BLACK_ROOK)
                attacks = board.get_rook_attacks(source_square, board.all_pieces);
            else // Queen
                attacks = board.get_bishop_attacks(source_square, board.all_pieces) |
                          board.get_rook_attacks(source_square, board.all_pieces);

            attacks &= ~own_pieces_mask;

            while (attacks)
            {
                int target_square = __builtin_ctzll(attacks);
                int captured_piece = board.board_arr[target_square];
                moves.emplace_back(source_square, target_square, piece_type, captured_piece);
                attacks &= attacks - 1;
            }
            pieces_mask &= pieces_mask - 1;
        }
    }
}