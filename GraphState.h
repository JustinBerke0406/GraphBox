#pragma once

#include <cmath>
#include <array>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include "graph/Node.h"

class GraphState {
private:
    class PositionGrid {
    private:
        float xOrigin, yOrigin, zoomScale, xResize, yResize;
    public:
        PositionGrid();

        const int WIDTH = 1920, HEIGHT = 1080;

        void zoom(float scale);
        void zoom(float scale, float x, float y);
        void zoom(float scale, sf::Event::MouseButtonEvent buttonEvent);

        void pan(float x, float y);
        void panRelative(float x, float y);

        [[nodiscard]] std::array<float, 2> gl_loc(int x, int y) const;
        [[nodiscard]] std::array<float, 2> gl_loc(sf::Vector2<int> pos) const;
        [[nodiscard]] std::array<float, 2> gl_loc(sf::Event::MouseButtonEvent buttonEvent) const;

        [[nodiscard]] std::array<int, 2> loc_gl(float x, float y) const;

        void transform(sf::Transformable& shape);
        void transform(sf::CircleShape& shape);
        void transform(sf::Text &shape);

        void updateResizeData(sf::Event::SizeEvent size);
    };

    PositionGrid positionGrid;

    std::vector<Node*> nodes;

    Node* selectedNode = nullptr;
public:
    GraphState() = default;

    PositionGrid& getPositionGrid();

    bool createNode(std::string label, float x, float y);
    bool createNode(std::string label, sf::Event::MouseButtonEvent event);

    bool deleteNode(std::string& label);

    void drawNodes();

    int nodeCount();

    bool selectNode(sf::Event::MouseButtonEvent event);
    void deselectNode();
    bool toggleNode(sf::Event::MouseButtonEvent event);

    bool cursorOverClickable();

    ~GraphState();
};