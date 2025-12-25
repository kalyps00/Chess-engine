#include "GameControler.hpp"
#include <iostream>

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

        if (isPromoting)
        {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;

                // Calculate clicked piece in promotion menu
                int file = promotionTarget % 8;
                int rank = promotionTarget / 8;
                int visual_rank = 7 - rank;

                int menu_x = file * BoardRender::SQUARE_SIZE;
                int start_y = visual_rank * BoardRender::SQUARE_SIZE;
                bool white = board.is_white_to_move();
                if (!white)
                    start_y = (visual_rank - 3) * BoardRender::SQUARE_SIZE;

                if (x >= menu_x && x < menu_x + BoardRender::SQUARE_SIZE &&
                    y >= start_y && y < start_y + 4 * BoardRender::SQUARE_SIZE)
                {
                    int clicked_index = (y - start_y) / BoardRender::SQUARE_SIZE;
                    int promotions[] = {
                        white ? WHITE_QUEEN : BLACK_QUEEN,
                        white ? WHITE_ROOK : BLACK_ROOK,
                        white ? WHITE_BISHOP : BLACK_BISHOP,
                        white ? WHITE_KNIGHT : BLACK_KNIGHT};

                    int selected_piece = promotions[clicked_index];

                    for (const auto &move : validMoves)
                    {
                        if (move.source == promotionSource && move.target == promotionTarget && move.promotion == selected_piece)
                        {
                            board.make_move(move);
                            check_game_over();
                            break;
                        }
                    }
                    isPromoting = false;
                }
                else
                {
                    // Clicked outside, cancel promotion
                    isPromoting = false;
                }
            }
            continue; // Skip other events while promoting
        }

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

                            validMoves.clear();
                            const std::vector<Move> &legal_moves = board.get_legal_moves();
                            for (const auto &move : legal_moves)
                            {
                                if (move.source == square)
                                {
                                    validMoves.push_back(move);
                                }
                            }
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
                                if (move.promotion)
                                {
                                    isPromoting = true;
                                    promotionSource = draggedSquare;
                                    promotionTarget = destSquare;
                                    break;
                                }
                                else
                                {
                                    board.make_move(move);
                                    check_game_over();
                                    break;
                                }
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

    if (isPromoting)
    {
        view.draw_promotion_menu(window, promotionTarget, board.is_white_to_move());
    }

    window.display();
}

void GameControler::check_game_over()
{
    Status status = board.get_game_status();
    if (status != ONGOING)
    {
        if (status == WHITE_WON)
            std::cout << "White Won!" << '\n';
        else if (status == BLACK_WON)
            std::cout << "Black Won!" << '\n';
        else if (status == DRAW)
            std::cout << "Draw!" << '\n';
    }
}
