#pragma once

#include <SFML/System/Vector2.hpp>
#include "graph/Node.h"
#include <windows.h>

const std::string VERSION = "1.1.2";

int launch();

void registerMovement();

void onMouseHeld(sf::Vector2f offset, Node* node);

void print();

void updater();