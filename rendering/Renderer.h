#pragma once

#include "../Single.h"

class Renderer {
    void drawEdge(sf::Vector2f pos1, sf::Vector2f pos2, int n1, int n2, char arrow);

    int textBlinker = 0;
public:
    void drawState();
    void draw();

    void operator ()();
};