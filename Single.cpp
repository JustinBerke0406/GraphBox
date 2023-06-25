#include <iostream>
#include "Single.h"

Single::Single() {
    state = new GraphState();

    window.create(sf::VideoMode(WIDTH, HEIGHT),
                  "GraphBox - v1.0");

    window.setFramerateLimit(60);

    if (!font.loadFromFile("resources/arial.ttf"))
        std::cout << "Unable to load arial.ttf";

    state->points=sf::VertexArray(sf::Points, WIDTH * HEIGHT);

    defaultView.reset(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    toolView.reset(sf::FloatRect(0, 0, WIDTH, TOOL_HEIGHT));

    float toolHeightPerc = (float)TOOL_HEIGHT / HEIGHT;

    defaultView.setViewport(sf::FloatRect(0.0f, toolHeightPerc, 1.0f, 1.0f));
    toolView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, toolHeightPerc));
}

Single::~Single() {
    delete state;
}
