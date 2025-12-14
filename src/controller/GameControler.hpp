#include "Board.hpp"
#include "BoardRender.hpp"
#include <SFML/Graphics.hpp>
class GameControler
{
private:
    sf::RenderWindow window;
    Board board;
    BoardRender view;

    // Drag and drop state
    bool isDragging = false;
    int draggedSquare = -1;
    int draggedPiece = 0;
    sf::Vector2i currentMousePos;

    void render();
    void handleEvents();

public:
    GameControler();
    void run();
};