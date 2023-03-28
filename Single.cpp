#include "Single.h"

Single::Single() {
    state = new GraphState();

    window.create(sf::VideoMode(state->getPositionGrid().WIDTH, state->getPositionGrid().HEIGHT),
                  "GraphBox - v1.0");
}

Single::~Single() {
    delete state;
}
