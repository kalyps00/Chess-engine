#include "Board.hpp"
#include "BoardRender.hpp"
#include <SFML/Graphics.hpp>
class GameControler
{
private:
    sf::RenderWindow window;
    Board board;
    BoardRender view;
    void render();
    void handleEvents();

public:
    GameControler();
    void run();
};