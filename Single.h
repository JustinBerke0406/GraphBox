#pragma once

#include <SFML/Graphics.hpp>
#include <windows.h>
#include "graph/GraphState.h"
#include "helper/InputHelper.h"
#include "PhysicsEngine.h"
#include "ModeHandler.h"
#include "option_types/Checkbox.h"
#include "option_types/Slider.h"

class Single {
private:
    Single();
public:
    static Single& instance() {
        static Single inst;
        return inst;
    }

    int WIDTH = 1920, HEIGHT = 1080;
    float OP_WIDTH_PER = 0.3125, OP_HEIGHT_PER = 0.8333;

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

    ModeHandler mode;

    const int THREADS = 15;

    const std::string OUTPUT_FILE = ".\\prints\\";

    int files = 0;

    bool oneIndexing = false;
    bool indexOrdering = true;

    std::map<std::string, Checkbox> checkSettings = {
            {"oneIndexing", Checkbox(200, 345, false)},
            {"forceInverse", Checkbox(200, 245, false)},
    };

    std::map<std::string, Slider> sliderSettings = {
            {"baseFriction", Slider(350, 100, 0, 250, physicsEngine.frictionMult * 100)},
            {"edgeFriction", Slider(350, 150, 0, 250, physicsEngine.spFrictionMult * 100)},
            {"restLength", Slider(350, 200, 0, 10, physicsEngine.springRestLen)},
    };

    sf::RenderWindow window;
    GraphState* state;
    sf::Font font;

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