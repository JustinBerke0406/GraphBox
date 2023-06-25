#pragma once

#include <cmath>
#include <array>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <thread>
#include "graph/Node.h"

class GraphState {
private:
    std::vector<Node*> nodes;

    Node* selectedNode = nullptr;

    int textBlinker = 0;
public:
    GraphState() = default;

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
    void toggleDensity();
    void invertForce();

    bool isNodeSelected();

    Node* getSelectedNode();

    void changeNodePosition(Node* node, sf::Event::MouseButtonEvent event);
    void changeNodePosition(Node* node, sf::Vector2i mousePos);
    void changeNodePositionLocally(Node* node, sf::Vector2f pos);

    float distance(Node* one, Node* two);
    float distanceSq(Node* &one, sf::Vector2i& two);

    std::vector<Node*> getNodes();

    int getNodeIndex(Node* node);

    void physicsUpdate();

    [[nodiscard]] std::string getMode() const;

    enum Mode { Edit, Connect, View, Typing };

    Mode mode = Mode::Edit;

    bool directed = false;
    bool errorLabel = false;
    bool forceMode = false;
    bool inverseForce = false;
    bool densityMode = false;
    bool adjMode = false;

    sf::VertexArray points;

    void drawDensityMap();
    void densityThreadCaller();

    ~GraphState();

    sf::Color gradient(float weight);

    void toggleAdjMode();

    bool adjacentTo(const Node *pNode, const Node *mainNode);

    void initToolbox();

    std::string getButtonAtPoint(sf::Vector2i vector2);

    void reset();
};