#pragma once

#include <SFML/System/Vector2.hpp>
#include "graph/Node.h"

class PhysicsEngine {
    static float distance(Node* one, Node* two);
public:
    constexpr static float REP_CONST = 1.0f,
            SPRING_CONST = 0.001f,
            SPRING_FRICTION = 0.01f,
            NORM_FRICTION = 0.01f;

    float frictionMult = 0.5f;
    float spFrictionMult = 0.0f;
    float springRestLen = 5.0f;

    void update();

    static void freezeAll();
};
