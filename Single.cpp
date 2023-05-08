#include <iostream>
#include "Single.h"

Single::Single() {
    state = new GraphState();

    window.create(sf::VideoMode(state->getPositionGrid().WIDTH, state->getPositionGrid().HEIGHT),
                  "GraphBox - v1.0");

    window.setFramerateLimit(60);

    if (!font.loadFromFile("resources/arial.ttf"))
        std::cout << "Unable to load arial.ttf";
}

Single::~Single() {
    delete state;
}
