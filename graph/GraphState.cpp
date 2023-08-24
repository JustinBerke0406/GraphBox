#include "GraphState.h"

#include <cmath>
#include <utility>
#include <iostream>
#include "../Single.h"

std::string GraphState::createNode(std::string label, float x, float y) {
    Node* node = new Node();
    node->x = x;
    node->y = y;
    node->label = std::move(label);

    nodes.push_back(node);

    return label;
}

std::string GraphState::createNode(std::string label, sf::Event::MouseButtonEvent event) {
    Single& single = Single::instance();

    auto pos = single.window.mapPixelToCoords(sf::Vector2i(event.x, event.y));

    return createNode(std::move(label), pos.x, pos.y);
}

std::string GraphState::createNode(sf::Event::MouseButtonEvent event) {
    for (int i = 0; i < nodes.size(); i++)
        if (!isLabelTaken(std::to_string(i)))
            return createNode(std::to_string(i), event);

    return createNode(std::to_string(nodes.size()), event);
}

std::string GraphState::createNode(sf::Vector2f pos) {
    int indexStart = Single::instance().oneIndexing;

    for (int i = indexStart; i <= nodes.size() + indexStart; i++)
        if (!isLabelTaken(std::to_string(i)))
            return createNode(std::to_string(i), pos.x, pos.y);

    return createNode(std::to_string(nodes.size()), pos.x, pos.y);
}

bool GraphState::deleteNode(Node* node) {
    for (int i = 0; i < nodes.size(); i++)
        if (nodes[i] == node) {
            for (Node* n : nodes)
                removeConnection(node, n);
            node->connections.clear();

            if (selectedNode == node)
                selectedNode = nullptr;

            delete node;
            nodes.erase(nodes.begin() + i);
            return true;
        }

    return false;
}

int GraphState::nodeCount() {
    return nodes.size();
}

bool GraphState::selectNode(sf::Event::MouseButtonEvent event) {
    Node* node = nodeAt(event);

    if (node == nullptr)
        return false;

    selectedNode = node;
    return true;
}

bool GraphState::selectNode(Node* node) {
    if (node == nullptr)
        return false;

    selectedNode = node;
    return true;
}

void GraphState::deselectNode() {
    selectedNode = nullptr;
}

bool GraphState::toggleNode(sf::Event::MouseButtonEvent event) {
    Node* curNode = selectedNode;

    if (!selectNode(event))
        return false;

    if (curNode == selectedNode)
        deselectNode();

    return true;
}

bool GraphState::toggleNode(Node* node) {
    if (selectedNode == node)
        deselectNode();
    else
        selectedNode = node;

    return true;
}

GraphState::~GraphState() {
    for (Node* n : nodes)
        delete n;

    nodes.clear();

    selectedNode = nullptr;
}

Node* GraphState::nodeAt(sf::Event::MouseButtonEvent event) {
    Single& single = Single::instance();

    auto pos = single.window.mapPixelToCoords(sf::Vector2i(event.x, event.y));

    for (int i = nodes.size() - 1; i >= 0; i--) {
        Node& n = *nodes[i];

        if (sqrt(pow(pos.x - n.x, 2)+pow(pos.y - n.y, 2)) <= 0.5*single.NODE_SIZE)
            return nodes[i];
    }

    return nullptr;
}

Node* GraphState::nodeAt(sf::Vector2<int> pos) {
    Single& single = Single::instance();

    auto post = single.window.mapPixelToCoords(pos);

    for (int i = nodes.size() - 1; i >= 0; i--) {
        Node& n = *nodes[i];

        if (sqrt(pow(post.x - n.x, 2)+pow(post.y - n.y, 2)) <= 0.5*single.NODE_SIZE)
            return nodes[i];
    }

    return nullptr;
}

bool GraphState::isLabelTaken(const std::string& str) {
    return std::any_of(nodes.begin(), nodes.end(), [str](Node* n) {return n->label == str;});
}

bool GraphState::isLabelTaken(const std::string &str, Node* exc) {
    return std::any_of(nodes.begin(), nodes.end(), [str, exc](Node* n) {return (n->label == str && n != exc);});
}

bool GraphState::wouldSelect(sf::Event::MouseButtonEvent event) {
    Node* n = nodeAt(event);

    if (n != nullptr && n != selectedNode)
        return true;

    return false;
}

bool GraphState::addConnection(Node* to, Node* from) {
    Node* addTo;

    if (from == nullptr) {
        if (selectedNode == nullptr) return false;

        addTo = selectedNode;
    }
    else
        addTo = from;

    if (std::any_of(addTo->connections.begin(), addTo->connections.end(), [to](Node* n) {return n == to;})) return false;

    if (!Single::instance().mode["directed"] && std::any_of(to->connections.begin(), to->connections.end(), [addTo](Node* n) {return n == addTo;})) return false;

    addTo->connections.push_back(to);

    return true;
}

bool GraphState::removeConnection(Node* to, Node* from) {
    Node* removeFrom;

    if (from == nullptr) {
        if (selectedNode == nullptr) return false;

        removeFrom = selectedNode;
    }
    else
        removeFrom = from;

    for (int i = 0; i < removeFrom->connections.size(); i++) {
        if (removeFrom->connections[i] == to) {
            removeFrom->connections.erase(removeFrom->connections.begin() + i);
            return true;
        }
    }

    return false;
}

bool GraphState::isNodeSelected() {
    return selectedNode != nullptr;
}

Node *GraphState::getSelectedNode() {
    return selectedNode;
}

void GraphState::changeNodePosition(Node* node, sf::Event::MouseButtonEvent event) {
    Single& single = Single::instance();

    auto pos = single.window.mapPixelToCoords(sf::Vector2i(event.x, event.y));

    node->x = pos.x;
    node->y = pos.y;
}

void GraphState::changeNodePositionLocally(Node *node, sf::Vector2f pos) {
    node->x = pos.x;
    node->y = pos.y;
}

void GraphState::changeNodePosition(Node *node, sf::Vector2i mousePos) {
    Single& single = Single::instance();

    auto pos = single.window.mapPixelToCoords(mousePos);

    node->x = pos.x;
    node->y = pos.y;
}

std::vector<Node*>& GraphState::getNodes() {
    return nodes;
}

int GraphState::getNodeIndex(Node *node) {
    for (int i = 0; i < nodes.size(); i++) {
        if (node == nodes[i])
            return i + Single::instance().oneIndexing;
    }

    return -1;
}

bool GraphState::adjacentTo(const Node *pNode, const Node *mainNode) const {
    if (pNode == mainNode)
        return false;

    for (Node* subNodes : mainNode->connections)
        if (subNodes == pNode)
            return true;

    if (!Single::instance().mode["directed"])
        for (Node* subNodes : pNode->connections)
            if (subNodes == mainNode)
                return true;

    return false;
}

void GraphState::reset() {
    Single& single = Single::instance();

    for (Node* n : nodes)
        delete n;

    selectedNode = nullptr;

    nodes.clear();

    single.defaultView.reset(sf::FloatRect(0, 0, single.WIDTH, single.HEIGHT));
    single.defaultView.setViewport(sf::FloatRect(0.0f, (float)single.TOOL_HEIGHT / single.HEIGHT, 1.0f, 1.0f));

    single.mode.clear();

    single.window.setTitle("Untitled - GraphBox");
    single.fileName = "";
}
