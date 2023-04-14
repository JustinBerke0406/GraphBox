#pragma once

#include <string>
#include <vector>

struct Node {
    float x, y;
    std::string label;
    std::vector<Node*> connections;
};
