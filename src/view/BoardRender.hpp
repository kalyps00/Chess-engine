#ifndef BOARD_RENDER_HPP
#define BOARD_RENDER_HPP
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Types.hpp>
#include <Board.hpp>
#include <vector>

class BoardRender
{

private:
    void draw_squares(sf::RenderWindow &window);
    sf::Sprite piece_sprite[12 + 1];
    sf::Texture chess_piece_texture;

public:
    static const int SQUARE_SIZE = 80;
    static const int PIECE_SIZE = 160;
    BoardRender();
    void draw_board(sf::RenderWindow &window, Board &board, int hide_square = -1, const std::vector<int> &highlight_squares = {});
    void draw_dragged_piece(sf::RenderWindow &window, int piece_type, sf::Vector2i position);
    int get_square_from_pixel(int x, int y);
};
#endif