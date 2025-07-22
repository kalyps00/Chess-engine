#include "GameControler.hpp"

GameControler::GameControler() : window(sf::VideoMode(1000, 1000), "Chess"),
                                 view(), board()
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
        // tbi mouseclick drag and drop etc
    }
}
void GameControler::render()
{
    window.clear(sf::Color(128, 128, 128));
    view.draw_board(window, board);
    window.display();
}
