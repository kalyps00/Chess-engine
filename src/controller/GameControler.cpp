#include "GameControler.hpp"

GameControler::GameControler() : window(sf::VideoMode(800, 800), "Chess"),
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
                }
    }
}
void GameControler::render()
{
    window.clear(sf::Color(128, 128, 128));
    view.draw_board(window, board);
    window.display();
}
