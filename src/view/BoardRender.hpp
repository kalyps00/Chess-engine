#ifndef BOARD_RENDER_HPP
#define BOARD_RENDER_HPP
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Types.hpp>
#include <Board.hpp>
class BoardRender
{
private:
    void draw_squares(sf::RenderWindow &window);
    sf::Sprite piece_sprite[12 + 1];
    sf::Texture chess_piece_texture;

public:
    BoardRender();
    void draw_board(sf::RenderWindow &window, Board &board);
};
#endif