#pragma once

#include <SFML/Graphics.hpp>
#include <windows.h>
#include "graph/GraphState.h"
#include "helper/InputHelper.h"
#include "PhysicsEngine.h"

class Single {
private:
    Single();
public:
    static Single& instance() {
        static Single inst;
        return inst;
    }

    const int WIDTH = 1920, HEIGHT = 1080;
    const int OP_WIDTH = 600, OP_HEIGHT = 900;

    const float ZOOM_SPEED = 1.05f, MOVE_SPEED = 10.35f, WHEEL_SENS = 1.3f;

    const sf::Color NODE_COLOR = sf::Color(240, 240, 240),
                    HIGHLIGHT_COLOR = sf::Color(240, 233, 121),
                    ERROR_COLOR = sf::Color(200, 30, 30),
                    ADJ_COLOR = sf::Color(44, 230, 2);

    const float EDGE_THICKNESS = 0.02f, ARROW_THICKNESS = 10.0f;
    const float NODE_SIZE = 90.0f;

    static const int HOLD_TIME = 250, DOUBLE_CLICK_TIME = 180;

    const int WINDOW_TEXT_SIZE = 16,
              WINDOW_TEXT_X_OFF = 10,
              WINDOW_TEXT_Y_OFF = 45;

    const int TEXT_BLINKER_TIME = 30;

    const float DELTA_TIME = 0.01f;

    PhysicsEngine physicsEngine;

    const int THREADS = 15;

    const GraphState::Mode DEFAULT_MODE = GraphState::Mode::Edit;

    const std::string OUTPUT_FILE = ".\\prints\\";

    int files = 0;

    bool indexOrdering = true;

    sf::RenderWindow window;
    GraphState* state;
    sf::Font font;

    sf::Vector3i gradOne = {0, 200, 54};
    sf::Vector3i gradTwo = {240, 240, 0};
    sf::Vector3i gradThree = {255, 0, 0};

    const int TOOL_HEIGHT = 40;

    sf::View defaultView;
    sf::View toolView;
    sf::View opView;

    std::vector<std::string> TOOL_BUTTONS = {"Save As", "Save", "Load", "Print", "Options", "Connect", "Directed", "Force", "Adjacency", "New"};

    OPENFILENAME ofn = {0};
    char szFile[260];

    std::string fileName;

    InputHelper inputHelper;

    ~Single();
};