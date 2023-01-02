#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <random>
#include "../include/config.hpp"
#include "../include/utils.hpp"
#include "../include/world.hpp"

int main()
{
    generator.seed(time(0));

    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tree Propagation 2D");
    // Create a viewport
    sf::View viewport(sf::Vector2f((config::grid_edge_size * config::block_edge_size) / 2, (config::grid_edge_size * config::block_edge_size) / 2), sf::Vector2f(config::block_edge_size * 10, config::block_edge_size * 10));
    window.setView(viewport);

    World grid(&window);

    float total_run_time = 0;
    float recent_frame_time = 0;
    int ticks = 0;

    sf::Clock clock;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))viewport.move(-1.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))viewport.move(1.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))viewport.move(0.f, -1.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))viewport.move(0.f, 1.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))viewport.zoom(1.01);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))viewport.zoom(0.99);

        window.setView(viewport);

        // clear the window with black color
        window.clear(sf::Color::Black);

        recent_frame_time = clock.restart().asMicroseconds();
        total_run_time += ((float) recent_frame_time / 1000 / 1000);    
        // std::cout << ((float) recent_frame_time / 1000) << " Frames Per Second\r";
        // draw everything here...
        grid.step((int) total_run_time);
        grid.draw();

        // end the current frame
        window.display();
    }

    return 0;
}