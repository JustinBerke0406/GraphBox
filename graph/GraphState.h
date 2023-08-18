#pragma once

#include <cmath>
#include <array>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <thread>
#include "Node.h"

class GraphState {
private:
    std::vector<Node*> nodes;

    Node* selectedNode = nullptr;
public:
    GraphState() = default;

    std::string createNode(std::string label, float x, float y);
    std::string createNode(std::string label, sf::Event::MouseButtonEvent event);
    std::string createNode(sf::Event::MouseButtonEvent event);
    std::string createNode(sf::Vector2f pos);

    bool deleteNode(Node* node);

    Node* nodeAt(sf::Event::MouseButtonEvent event);
    Node* nodeAt(sf::Vector2<int> pos);

    int nodeCount();

    bool selectNode(sf::Event::MouseButtonEvent event);
    bool selectNode(Node* node);

    void deselectNode();

    bool toggleNode(sf::Event::MouseButtonEvent event);
    bool toggleNode(Node* node);

    bool wouldSelect(sf::Event::MouseButtonEvent event);

    bool isLabelTaken(const std::string& str);
    bool isLabelTaken(const std::string& str, Node* exc);

    bool addConnection(Node* to, Node* from = nullptr);
    bool removeConnection(Node* to, Node* from = nullptr);

    bool isNodeSelected();

    Node* getSelectedNode();

    void changeNodePosition(Node* node, sf::Event::MouseButtonEvent event);
    void changeNodePosition(Node* node, sf::Vector2i mousePos);
    void changeNodePositionLocally(Node* node, sf::Vector2f pos);

    std::vector<Node*>& getNodes();

    int getNodeIndex(Node* node);

    sf::VertexArray points;

    ~GraphState();

    bool adjacentTo(const Node *pNode, const Node *mainNode) const;

    void reset();
};