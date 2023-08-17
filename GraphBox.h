#pragma once

#include <SFML/System/Vector2.hpp>
#include "graph/Node.h"
#include <windows.h>

const std::string VERSION = "1.0.0";

int launch();

void registerMovement();

void onMouseHeld(sf::Vector2f offset, Node* node);

void print();

void render();

void loadFile();
void saveFile();

void launchSettings();

void updater();

std::string fileName(std::string path);