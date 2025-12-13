#include "BoardRender.hpp"
#include "Types.hpp"
#include <iostream>

float square_size = 100;
float piece_size = 160;
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
        piece_sprite[WHITE_KING].setTextureRect(sf::IntRect(0 * piece_size, 0, piece_size, piece_size));
        piece_sprite[WHITE_QUEEN].setTextureRect(sf::IntRect(1 * piece_size, 0, piece_size, piece_size));
        piece_sprite[WHITE_BISHOP].setTextureRect(sf::IntRect(2 * piece_size, 0, piece_size, piece_size));
        piece_sprite[WHITE_KNIGHT].setTextureRect(sf::IntRect(3 * piece_size, 0, piece_size, piece_size));
        piece_sprite[WHITE_ROOK].setTextureRect(sf::IntRect(4 * piece_size, 0, piece_size, piece_size));
        piece_sprite[WHITE_PAWN].setTextureRect(sf::IntRect(5 * piece_size, 0, piece_size, piece_size));
        // Black pieces
        piece_sprite[BLACK_KING].setTextureRect(sf::IntRect(0 * piece_size, piece_size, piece_size, piece_size));
        piece_sprite[BLACK_QUEEN].setTextureRect(sf::IntRect(1 * piece_size, piece_size, piece_size, piece_size));
        piece_sprite[BLACK_BISHOP].setTextureRect(sf::IntRect(2 * piece_size, piece_size, piece_size, piece_size));
        piece_sprite[BLACK_KNIGHT].setTextureRect(sf::IntRect(3 * piece_size, piece_size, piece_size, piece_size));
        piece_sprite[BLACK_ROOK].setTextureRect(sf::IntRect(4 * piece_size, piece_size, piece_size, piece_size));
        piece_sprite[BLACK_PAWN].setTextureRect(sf::IntRect(5 * piece_size, piece_size, piece_size, piece_size));

        // Scale to fit square size
        float piece_scale = square_size / piece_size;
        for (int i = 1; i <= 12; i++)
        {
            piece_sprite[i].setScale(piece_scale, piece_scale);
        }
    }
}
void BoardRender::draw_board(sf::RenderWindow &window, Board &board)
{
    sf::RectangleShape square;
    square.setSize(sf::Vector2f(square_size, square_size));
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            // square
            square.setPosition(file * square_size, rank * square_size);
            if ((rank + file) % 2 == 0)
                square.setFillColor(sf::Color(238, 238, 210));
            else
                square.setFillColor(sf::Color(118, 150, 86));
            window.draw(square);

            // piece
            int piece_type = board.get_piece_at(8 * rank + file);
            if (piece_type > 0 && piece_type <= 12)
            {
                piece_sprite[piece_type].setPosition(file * square_size, rank * square_size);
                window.draw(piece_sprite[piece_type]);
            }
        }
    }
}
