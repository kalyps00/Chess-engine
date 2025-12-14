#include "BoardRender.hpp"
#include "Types.hpp"
#include <iostream>

BoardRender::BoardRender()
{
    if (!chess_piece_texture.loadFromFile("assets/Chess_Pieces_Sprite2.png"))
    {
        std::cerr << "sprite not loaded" << std::endl;
    }
    else
    {
        for (int i = 1; i <= 12; i++)
        {
            piece_sprite[i].setTexture(chess_piece_texture);
        }
        // White pices
        piece_sprite[WHITE_KING].setTextureRect(sf::IntRect(0 * PIECE_SIZE, 0, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[WHITE_QUEEN].setTextureRect(sf::IntRect(1 * PIECE_SIZE, 0, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[WHITE_BISHOP].setTextureRect(sf::IntRect(2 * PIECE_SIZE, 0, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[WHITE_KNIGHT].setTextureRect(sf::IntRect(3 * PIECE_SIZE, 0, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[WHITE_ROOK].setTextureRect(sf::IntRect(4 * PIECE_SIZE, 0, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[WHITE_PAWN].setTextureRect(sf::IntRect(5 * PIECE_SIZE, 0, PIECE_SIZE, PIECE_SIZE));
        // Black pieces
        piece_sprite[BLACK_KING].setTextureRect(sf::IntRect(0 * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[BLACK_QUEEN].setTextureRect(sf::IntRect(1 * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[BLACK_BISHOP].setTextureRect(sf::IntRect(2 * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[BLACK_KNIGHT].setTextureRect(sf::IntRect(3 * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[BLACK_ROOK].setTextureRect(sf::IntRect(4 * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));
        piece_sprite[BLACK_PAWN].setTextureRect(sf::IntRect(5 * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));

        // Scale to fit square size
        float piece_scale = (float)SQUARE_SIZE / PIECE_SIZE;
        for (int i = 1; i <= 12; i++)
        {
            piece_sprite[i].setScale(piece_scale, piece_scale);
        }
    }
}
void BoardRender::draw_board(sf::RenderWindow &window, Board &board, int hide_square, const std::vector<int> &highlight_squares)
{
    sf::RectangleShape square;
    square.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));

    sf::CircleShape highlight(SQUARE_SIZE / 6);
    highlight.setFillColor(sf::Color(100, 100, 100, 150));
    highlight.setOrigin(highlight.getRadius(), highlight.getRadius());

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            // Logic: rank 0 is bottom (A1), rank 7 is top (A8)
            // Visual: y=0 is top, y=max is bottom
            // So visual_rank = 7 - logical_rank
            int current_square = 8 * rank + file;
            int visual_rank = 7 - rank;

            // square
            square.setPosition(file * SQUARE_SIZE, visual_rank * SQUARE_SIZE);
            if ((rank + file) % 2 != 0)
                square.setFillColor(sf::Color(238, 238, 210));
            else
                square.setFillColor(sf::Color(118, 150, 86));
            window.draw(square);

            // highlight
            for (int s : highlight_squares)
            {
                if (s == current_square)
                {
                    highlight.setPosition(file * SQUARE_SIZE + SQUARE_SIZE / 2, visual_rank * SQUARE_SIZE + SQUARE_SIZE / 2);
                    window.draw(highlight);
                    break;
                }
            }

            // dragged piece
            if (current_square == hide_square)
                continue;

            int piece_type = board.get_piece_at(current_square);
            if (piece_type > 0 && piece_type <= 12)
            {
                piece_sprite[piece_type].setPosition(file * SQUARE_SIZE, visual_rank * SQUARE_SIZE);
                window.draw(piece_sprite[piece_type]);
            }
        }
    }
}

void BoardRender::draw_dragged_piece(sf::RenderWindow &window, int piece_type, sf::Vector2i position)
{
    if (piece_type > 0 && piece_type <= 12)
    {
        piece_sprite[piece_type].setPosition(
            position.x - SQUARE_SIZE / 2,
            position.y - SQUARE_SIZE / 2);
        window.draw(piece_sprite[piece_type]);
    }
}
