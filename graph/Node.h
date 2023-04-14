#pragma once

#include <string>
#include <vector>

struct Node {
    float x, y;
    std::string label;
    std::vector<Node*> connections;

    sf::Vector2f velocity = sf::Vector2f(0,0);
};
