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
                    HIGHLIGHT_COLOR = sf::Color(240, 233, 121),
                    ERROR_COLOR = sf::Color(200, 30, 30);

    const float EDGE_THICKNESS = 0.02f, ARROW_THICKNESS = 10.0f;

    const int MOUSE_HOLD_TIME = 3, DOUBLE_CLICK_TIME = 7;

    const int WINDOW_TEXT_SIZE = 16,
              WINDOW_TEXT_X_OFF = 10,
              WINDOW_TEXT_Y_OFF = 5;

    const int TEXT_BLINKER_TIME = 30;

    const float REP_CONST = 1.0f,
                SPRING_CONST = 1.0f,
                SPRING_REST_LEN = 5.0f,
                SPRING_FRICTION = 0.01f,
                DELTA_TIME = 0.01f;

    const GraphState::Mode DEFAULT_MODE = GraphState::Mode::Edit;

    sf::RenderWindow window;
    GraphState* state;
    sf::Font font;

    ~Single();
};