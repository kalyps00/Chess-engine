#include "Board.hpp"
#include "BoardRender.hpp"
#include "../model/MoveGenerator.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

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
    std::vector<Move> validMoves;

    // Promotion state
    bool isPromoting = false;
    int promotionSource = -1;
    int promotionTarget = -1;

    void render();
    void handleEvents();

public:
    GameControler();
    void run();
};