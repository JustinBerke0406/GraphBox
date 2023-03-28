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

    const float ZOOM_SPEED = 1.0005f, MOVE_SPEED = 0.0035f;

    sf::RenderWindow window;
    GraphState* state;

    ~Single();
};