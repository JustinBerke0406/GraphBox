#pragma once

#include <SFML/System/Vector2.hpp>
#include "graph/Node.h"

int launch();

void registerMovement();

void onMouseHeld(sf::Vector2f offset, Node* node);

void render();