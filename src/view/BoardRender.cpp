#include "BoardRender.hpp"
#include "Types.hpp"

void BoardRender::draw_squares(sf::RenderWindow &window)
{
    sf::RectangleShape square;
    square.setSize(sf::Vector2f(100.0f, 100.0f));
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            square.setPosition(file * 100, rank * 100);
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

    draw_squares(window);
    sf::Texture chess_piece_texture;
    chess_piece_texture.loadFromFile("assets/Chess_Pieces_Sprite.png");
}