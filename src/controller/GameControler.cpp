#include "GameControler.hpp"

GameControler::GameControler() : window(sf::VideoMode(800, 800), "Chess"),
                                 board(), view()
{
    window.setFramerateLimit(60);
};

void GameControler::run()
{
    sf::Event event;
    while (window.isOpen())
    {
        if (window.waitEvent(event))
        {
            handleEvent(event);
            render();
        }
    }
}

void GameControler::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::Closed)
        window.close();
}
void GameControler::render()
{
    window.clear(sf::Color(128, 128, 128));
    view.draw_board(window, board);
    window.display();
}
