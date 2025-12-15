#include "GameControler.hpp"

GameControler::GameControler() : window(sf::VideoMode(BoardRender::SQUARE_SIZE * 8, BoardRender::SQUARE_SIZE * 8), "Chess"),
                                 board(), view()
{
    window.setFramerateLimit(60);
};

void GameControler::run()
{
    while (window.isOpen())
    {
        handleEvents();
        render();
    }
}

void GameControler::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                int square = view.get_square_from_pixel(event.mouseButton.x, event.mouseButton.y);

                if (square >= 0 && square < 64)
                {
                    int piece = board.get_piece_at(square);
                    if (piece != EMPTY)
                    {
                        bool isWhiteTurn = board.is_white_to_move();
                        if ((isWhiteTurn && is_white_piece(piece)) || (!isWhiteTurn && is_black_piece(piece)))
                        {
                            isDragging = true;
                            draggedPiece = piece;
                            draggedSquare = square;
                            currentMousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);

                            validMoves = MoveGenerator::generate_moves(board);
                        }
                    }
                }
            }
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (isDragging)
                {
                    int destSquare = view.get_square_from_pixel(event.mouseButton.x, event.mouseButton.y);

                    if (destSquare >= 0 && destSquare < 64)
                    {
                        for (const auto &move : validMoves)
                        {
                            if (move.source == draggedSquare && move.target == destSquare)
                            {
                                board.make_move(move);
                                break;
                            }
                        }
                    }

                    isDragging = false;
                    draggedSquare = -1;
                    draggedPiece = 0;
                }
            }
        }

        if (event.type == sf::Event::MouseMoved)
        {
            if (isDragging)
            {
                currentMousePos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
            }
        }
    }
}
void GameControler::render()
{
    window.clear(sf::Color(128, 128, 128));

    std::vector<int> highlight_squares;
    if (isDragging)
    {
        for (const auto &move : validMoves)
        {
            if (move.source == draggedSquare)
            {
                highlight_squares.push_back(move.target);
            }
        }
    }

    view.draw_board(window, board, isDragging ? draggedSquare : -1, highlight_squares);

    if (isDragging)
    {
        view.draw_dragged_piece(window, draggedPiece, currentMousePos);
    }

    window.display();
}
