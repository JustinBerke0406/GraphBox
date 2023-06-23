#include <iostream>
#include "Single.h"

Single::Single() {
    state = new GraphState();

    window.create(sf::VideoMode(state->getPositionGrid().WIDTH, state->getPositionGrid().HEIGHT),
                  "GraphBox - v1.0");

    window.setFramerateLimit(60);

    if (!font.loadFromFile("resources/arial.ttf"))
        std::cout << "Unable to load arial.ttf";

    state->points=sf::VertexArray(sf::Points, state->getPositionGrid().WIDTH * state->getPositionGrid().HEIGHT);

    defaultView.reset(sf::FloatRect(0, 0, state->getPositionGrid().WIDTH, state->getPositionGrid().HEIGHT));
    toolView.reset(sf::FloatRect(0, 0, state->getPositionGrid().WIDTH, toolViewHeight));

    float toolHeightPerc = (float)toolViewHeight/state->getPositionGrid().HEIGHT;

    defaultView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    toolView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, toolHeightPerc));
}

Single::~Single() {
    delete state;
}
