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

    const int WIDTH = 1920, HEIGHT = 1080;

    const float ZOOM_SPEED = 1.05f, MOVE_SPEED = 0.35f, WHEEL_SENS = 1.3f;

    const sf::Color NODE_COLOR = sf::Color(240, 240, 240),
                    HIGHLIGHT_COLOR = sf::Color(240, 233, 121),
                    ERROR_COLOR = sf::Color(200, 30, 30),
                    ADJ_COLOR = sf::Color(44, 230, 2);

    const float EDGE_THICKNESS = 0.02f, ARROW_THICKNESS = 10.0f;
    const float NODE_SIZE = 90.0f;

    const int MOUSE_HOLD_TIME = 3, DOUBLE_CLICK_TIME = 7;

    const int WINDOW_TEXT_SIZE = 16,
              WINDOW_TEXT_X_OFF = 10,
              WINDOW_TEXT_Y_OFF = 45;

    const int TEXT_BLINKER_TIME = 30;

    const float REP_CONST = 1.0f,
                SPRING_CONST = 1.0f,
                SPRING_REST_LEN = 5.0f,
                SPRING_FRICTION = 0.01f,
                NORM_FRICTION = 0.9f,
                DELTA_TIME = 0.01f;

    const int THREADS = 15;

    const GraphState::Mode DEFAULT_MODE = GraphState::Mode::Edit;

    const std::string OUTPUT_FILE = "GraphBox_Save_";

    int files = 0;

    bool indexOrdering = true;

    sf::RenderWindow window;
    GraphState* state;
    sf::Font font;

    sf::Vector3i gradOne = {0, 200, 54};
    sf::Vector3i gradTwo = {240, 240, 0};
    sf::Vector3i gradThree = {255, 0, 0};

    const int toolViewHeight = 40;

    sf::View defaultView;
    sf::View toolView;

    ~Single();
};