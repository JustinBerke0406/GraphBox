#pragma once

#include <SFML/Graphics.hpp>

class Single {
private:
    Single();
public:
    static Single& instance() {
        static Single inst;
        return inst;
    }

    int WIDTH = 1920, HEIGHT = 1080;

    sf::RenderWindow window;
};