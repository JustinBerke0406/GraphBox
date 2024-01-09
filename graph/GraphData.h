#pragma once

#include "Node.h"

class GraphData {
public:
    std::vector<Node*> degree;

    GraphData() = default;

    void updateDegrees();
    int uniqueDegrees();
};
