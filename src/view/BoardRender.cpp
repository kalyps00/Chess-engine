#include "BoardRender.hpp"
#include "Types.hpp"
#include <iostream>
float square_size = 90;
void BoardRender::draw_squares(sf::RenderWindow &window)
{
    sf::RectangleShape square;
    square.setSize(sf::Vector2f(square_size, square_size));
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            square.setPosition(file * square_size, rank * square_size);
            if ((rank + file) % 2 == 0)
                square.setFillColor(sf::Color(238, 238, 210));
            else
                square.setFillColor(sf::Color(118, 150, 86));
            window.draw(square);
        }
    }
}
void BoardRender::draw_board(sf::RenderWindow &window, Board &board)
{
    // draw_squares(window);
    sf::Texture chess_piece_texture;
    if (!chess_piece_texture.loadFromFile("assets/Chess_Pieces_Sprite.png"))
    {
        std::cerr << "sprite not loaded";
        return;
    }
    for (int i = 1; i <= 12; i++)
    {
        piece_sprite[i].setTexture(chess_piece_texture);
    }
    // White pices
    piece_sprite[WHITE_KING].setTextureRect(sf::IntRect(0 * 45, 0, 45, 45));
    piece_sprite[WHITE_QUEEN].setTextureRect(sf::IntRect(1 * 45, 0, 45, 45));
    piece_sprite[WHITE_BISHOP].setTextureRect(sf::IntRect(2 * 45, 0, 45, 45));
    piece_sprite[WHITE_KNIGHT].setTextureRect(sf::IntRect(3 * 45, 0, 45, 45));
    piece_sprite[WHITE_ROOK].setTextureRect(sf::IntRect(4 * 45, 0, 45, 45));
    piece_sprite[WHITE_PAWN].setTextureRect(sf::IntRect(5 * 45, 0, 45, 45));
    // Black pieces
    piece_sprite[BLACK_KING].setTextureRect(sf::IntRect(0 * 45, 45, 45, 45));
    piece_sprite[BLACK_QUEEN].setTextureRect(sf::IntRect(1 * 45, 45, 45, 45));
    piece_sprite[BLACK_BISHOP].setTextureRect(sf::IntRect(2 * 45, 45, 45, 45));
    piece_sprite[BLACK_KNIGHT].setTextureRect(sf::IntRect(3 * 45, 45, 45, 45));
    piece_sprite[BLACK_ROOK].setTextureRect(sf::IntRect(4 * 45, 45, 45, 45));
    piece_sprite[BLACK_PAWN].setTextureRect(sf::IntRect(5 * 45, 45, 45, 45));
    // board
    float piece_scale = 2;
    for (int i = 1; i <= 12; i++)
    {
        piece_sprite[i].setScale(piece_scale, piece_scale);
    }
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
            // probrably work need test but first board representation imp to make sure that it works
            int piece_type = board.get_piece_at(8 * rank + file);
            piece_sprite[piece_type].setPosition(file * square_size, rank * square_size);
            window.draw(piece_sprite[piece_type]);
        }
    }
    (void)board; // warning
}
