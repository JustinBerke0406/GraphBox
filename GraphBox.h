#pragma once

#include <SFML/System/Vector2.hpp>
#include "graph/Node.h"
#include <windows.h>

int launch();

void registerMovement();

void onMouseHeld(sf::Vector2f offset, Node* node);

void print();

void render();

void loadFile();
void saveFile();

std::string fileName(std::string path);