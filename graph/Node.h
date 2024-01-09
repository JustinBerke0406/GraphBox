#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>

struct Node {
    float x, y;
    int degrees = 0;
    std::string label;
    std::vector<Node*> connections;

    bool locked = false;

    sf::Vector2f velocity = sf::Vector2f(0,0);
    sf::Vector2f force = sf::Vector2f(0,0);

    bool directlyConnectsTo(Node* test) {
        return std::any_of(connections.begin(), connections.end(), [test] (auto adj) -> bool { return adj == test;});
    }
};
