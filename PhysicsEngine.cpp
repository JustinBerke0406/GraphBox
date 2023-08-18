#include "PhysicsEngine.h"
#include "Single.h"

void PhysicsEngine::update() {
    Single& single = Single::instance();

    GraphState* state = single.state;

    auto nodes = state->getNodes();

    auto mag = [](sf::Vector2f vc) {
        return (float)sqrt(pow(vc.x, 2) + pow(vc.y, 2));
    };

    auto velVectorRep = [&single](Node* one, Node* two) {
        auto dist = distance(one, two)/single.NODE_SIZE;
        auto unitVector = sf::Vector2f(one->x - two->x, one->y - two->y) / dist;
        return unitVector*REP_CONST/(float)pow(fmax(dist, single.NODE_SIZE), 2);};

    auto velVectorSpring = [&single, this](Node* one, Node* two) {
        auto dist = distance(one, two)/single.NODE_SIZE;
        auto unitVector = sf::Vector2f(one->x - two->x, one->y - two->y) / dist;
        auto spring = unitVector*SPRING_CONST*(float)pow(dist - springRestLen, 2);
        float sign = ((dist < springRestLen) ? 1.0f : -1.0f);

        return spring*sign;
    };

    for (Node* node1 : nodes) {
        for (Node* cons : node1->connections) {
            node1->velocity += velVectorSpring(node1, cons);
            cons->velocity += velVectorSpring(cons, node1);

            if (SPRING_FRICTION >= mag(node1->velocity))
                node1->velocity = sf::Vector2f(0, 0);
            else
                node1->velocity -= (node1->velocity/mag(node1->velocity))*SPRING_FRICTION*spFrictionMult;

            if (SPRING_FRICTION >= mag(cons->velocity))
                cons->velocity = sf::Vector2f(0, 0);
            else
                cons->velocity -= (cons->velocity/mag(cons->velocity))*SPRING_FRICTION*spFrictionMult;
        }

        for (Node* node2 : nodes) {
            if (node1 != node2) {
                node1->velocity += velVectorRep(node1, node2) * ((single.mode["inv"]) ? -1.f : 1.f);
            }
        }

        for (Node* node2 : nodes) {
            if (node1 != node2) {
                auto dist = distance(node1, node2);

                if (dist <= single.NODE_SIZE) {
                    auto vel = node1->velocity;
                    sf::Vector2f distVec(node2->x - node1->x, node2->y - node1->y);
                    auto unit = distVec/dist;

                    if ((vel.x * unit.x) + (vel.y * unit.y) >= 0) {
                        auto Va = node1->velocity - node2->velocity;
                        float scale = Va.x*unit.x + Va.y*unit.y;

                        sf::Vector2f Vaf = {Va.x - scale*unit.x, Va.y - scale*unit.y};

                        node1->velocity = Vaf + node2->velocity;
                        node2->velocity += scale * unit;
                    }
                }
            }
        }
    }

    for (Node* node : nodes) {
        if (node->locked) {
            node->velocity = {0, 0};
        }

        auto v = node->velocity;
        float magv = mag(v);

        if (magv == 0)
            continue;

        v *= NORM_FRICTION * frictionMult / magv;

        if (abs(v.x) >= abs(node->velocity.x)) {
            node->velocity = {0, 0};
        }
        else {
            node->velocity -= v;
        }

        node->x += node->velocity.x * single.DELTA_TIME * single.NODE_SIZE;
        node->y += node->velocity.y * single.DELTA_TIME * single.NODE_SIZE;
    }
}

float PhysicsEngine::distance(Node *one, Node *two) {
    return sqrt(pow(one->x-two->x, 2)+pow(one->y-two->y, 2));
}

void PhysicsEngine::freezeAll() {
    auto nodes = Single::instance().state->getNodes();

    for (Node* n : nodes)
        n->velocity = sf::Vector2f(0, 0);
}
