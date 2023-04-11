#pragma once

#include <SFML/Graphics.hpp>
#include "GraphState.h"

class Single {
private:
    Single();
public:
    static Single& instance() {
        static Single inst;
        return inst;
    }

    const float ZOOM_SPEED = 1.05f, MOVE_SPEED = 0.35f;

    const sf::Color NODE_COLOR = sf::Color(240, 240, 240),
                    HIGHLIGHT_COLOR = sf::Color(240, 233, 121);

    const float EDGE_THICKNESS = 0.02f;

    sf::RenderWindow window;
    GraphState* state;
    sf::Font font;

    ~Single();
};