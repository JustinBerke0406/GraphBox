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
        void zoom(float scale, sf::Event::MouseWheelScrollEvent buttonEvent);

        void pan(float x, float y);
        void panRelative(float x, float y);

        [[nodiscard]] std::array<float, 2> gl_loc(int x, int y) const;
        [[nodiscard]] std::array<float, 2> gl_loc(sf::Vector2<int> pos) const;
        [[nodiscard]] std::array<float, 2> gl_loc(sf::Event::MouseButtonEvent buttonEvent) const;

        [[nodiscard]] std::array<int, 2> loc_gl(float x, float y) const;

        void transform(sf::Transformable& shape);
        void transform(sf::CircleShape& shape);
        void transform(sf::Text &shape);
        void transform(sf::Vertex* ones, int count);

        void updateResizeData(sf::Event::SizeEvent size);
    };

    PositionGrid positionGrid;

    std::vector<Node*> nodes;

    Node* selectedNode = nullptr;

    int textBlinker = 0;
public:
    GraphState() = default;

    PositionGrid& getPositionGrid();

    std::string createNode(std::string label, float x, float y);
    std::string createNode(std::string label, sf::Event::MouseButtonEvent event);
    std::string createNode(sf::Event::MouseButtonEvent event);

    bool deleteNode(Node* node);

    Node* nodeAt(sf::Event::MouseButtonEvent event);
    Node* nodeAt(sf::Vector2<int> pos);

    void drawNodes();
    void drawEdge(sf::Vector2f pos1, sf::Vector2f pos2, int n1, int n2, char arrow);

    int nodeCount();

    bool selectNode(sf::Event::MouseButtonEvent event);
    bool selectNode(Node* node);

    void deselectNode();

    bool toggleNode(sf::Event::MouseButtonEvent event);
    bool toggleNode(Node* node);

    bool wouldSelect(sf::Event::MouseButtonEvent event);

    bool cursorOverClickable();

    bool isLabelTaken(const std::string& str);
    bool isLabelTaken(const std::string& str, Node* exc);

    bool addConnection(Node* to, Node* from = nullptr);
    bool removeConnection(Node* to, Node* from = nullptr);

    void toggleDirectedMode();
    void toggleConnectMode();
    void toggleForce();

    bool isNodeSelected();

    Node* getSelectedNode();

    void changeNodePosition(Node* node, sf::Event::MouseButtonEvent event);
    void changeNodePosition(Node* node, sf::Vector2i mousePos);
    void changeNodePositionLocally(Node* node, sf::Vector2f pos);

    float distance(Node* one, Node* two);

    std::vector<Node*> getNodes();

    int getNodeIndex(Node* node);

    void physicsUpdate();

    [[nodiscard]] std::string getMode() const;

    enum Mode { Edit, Connect, View, Typing };

    Mode mode = Mode::Edit;

    bool directed = false;
    bool errorLabel = false;
    bool forceMode = false;

    ~GraphState();
};