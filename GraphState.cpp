#include "GraphState.h"

#include <cmath>
#include <utility>
#include <iostream>
#include <fstream>
#include "Single.h"

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
    for (int i = 0; i < nodes.size(); i++)
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

void GraphState::drawNodes() {
    Single& single = Single::instance();

    std::vector<sf::CircleShape> circles;
    std::vector<sf::Text> texts;

    if (mode == Mode::Typing) {
        textBlinker++;

        if (textBlinker > single.TEXT_BLINKER_TIME*2)
            textBlinker = 0;
    }

    for (const Node* n : nodes) {
        sf::CircleShape circ;
        circ.setRadius(0.5f * single.NODE_SIZE);
        circ.setOrigin(circ.getLocalBounds().width/2, circ.getLocalBounds().height/2);
        circ.setOutlineThickness(-0.02f * single.NODE_SIZE);
        circ.setOutlineColor(sf::Color::Black);

        if (n->locked) {
            circ.setOutlineColor(sf::Color::Red);
        }

        circ.setPointCount(100);

        sf::Color fillColor = (n == selectedNode) ? ((!errorLabel) ? single.HIGHLIGHT_COLOR : single.ERROR_COLOR) : single.NODE_COLOR;

        if (adjMode && selectedNode != nullptr) {
            if (adjacentTo(n, selectedNode))
                fillColor = single.ADJ_COLOR;
        }

        circ.setFillColor(fillColor);
        circ.setPosition(n->x, n->y);

        circles.push_back(circ);

        sf::Text text;
        text.setFont(single.font);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::Black);
        text.setPosition(n->x, n->y);
        text.setString(n->label);
        text.setOrigin(text.getLocalBounds().width/2, 25);

        if (mode == Mode::Typing && textBlinker < single.TEXT_BLINKER_TIME && n == selectedNode && !n->label.empty()) {
            text.setString(n->label + "_");
        }

        texts.push_back(text);

        for (Node* con : n->connections)
            drawEdge(sf::Vector2f(n->x, n->y), sf::Vector2f(con->x, con->y), true, true, 2*directed);
    }

    for (int i = 0; i < texts.size(); i++) {
        single.window.draw(circles[i]);
        single.window.draw(texts[i]);
    }

    if (single.state->mode==GraphState::Mode::Connect
        && selectedNode != nullptr
        && nodeAt(sf::Mouse::getPosition(single.window)) != selectedNode) {

        auto mousePos = sf::Mouse::getPosition(single.window);

        auto mPos = single.window.mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y));

        drawEdge(sf::Vector2f(selectedNode->x, selectedNode->y), sf::Vector2f(mPos.x, mPos.y), true, false, 0);
    }

    sf::Text text;
    text.setFont(single.font);
    text.setCharacterSize(single.WINDOW_TEXT_SIZE);
    text.setFillColor(sf::Color::Black);
    text.setPosition(sf::Vector2f(single.WINDOW_TEXT_X_OFF, single.WINDOW_TEXT_Y_OFF));
    //text.setString("Mode: " + getMode());

    single.window.setView(single.window.getDefaultView());
    single.window.draw(text);
    single.window.setView(single.defaultView);
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

bool GraphState::cursorOverClickable() {
    Single& single = Single::instance();

    auto mousePos = sf::Mouse::getPosition(single.window);

    if (single.window.getViewport(single.defaultView).contains(mousePos)) {
        Node *node = nodeAt(mousePos);

        if (node == nullptr)
            return false;

        return true;
    }
    else if (single.window.getViewport(single.toolView).contains(mousePos)) {
        return true;
    }

    return false;
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

    if (!directed && std::any_of(to->connections.begin(), to->connections.end(), [addTo](Node* n) {return n == addTo;})) return false;

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

void GraphState::drawEdge(sf::Vector2f pos1, sf::Vector2f pos2, int n1, int n2, char arrow) {
    Single& single = Single::instance();

    sf::Vector2f mpos1, mpos2;

    if (pos1.x < pos2.x) {
        mpos1 = pos1;
        mpos2 = pos2;
    } else {
        mpos1 = pos2;
        mpos2 = pos1;

        bool temp = n1;
        n1 = n2;
        n2 = temp;

        if (arrow == 2)
            arrow = 1;
        else if (arrow == 1)
            arrow = 2;
    }

    sf::Vertex line[4];

    if (arrow == 1)
        n1 *= 2;
    else if (arrow == 2)
        n2 *= 2;

    float cX = mpos2.x - mpos1.x,
            cY = mpos2.y - mpos1.y;

    float mag = sqrt(pow(cX, 2) + pow(cY, 2));

    sf::Vector2f perp(-cY*single.EDGE_THICKNESS/mag, cX*single.EDGE_THICKNESS/mag);

    line[0].position = sf::Vector2f((cX*0.5f*single.NODE_SIZE/mag)*n1+mpos1.x - perp.x*single.NODE_SIZE, (cY*0.5f*single.NODE_SIZE/mag)*n1 + mpos1.y - perp.y*single.NODE_SIZE);
    line[0].color = sf::Color::Black;

    line[1].position = sf::Vector2f(-(cX*0.5f*single.NODE_SIZE/mag)*n2+mpos2.x - perp.x*single.NODE_SIZE, -(cY*0.5f*single.NODE_SIZE/mag)*n2+mpos2.y - perp.y*single.NODE_SIZE);
    line[1].color = sf::Color::Black;

    line[2].position = sf::Vector2f(line[1].position.x+perp.x*2*single.NODE_SIZE, line[1].position.y+perp.y*2*single.NODE_SIZE);
    line[2].color = sf::Color::Black;

    line[3].position = sf::Vector2f(line[0].position.x+perp.x*2*single.NODE_SIZE, line[0].position.y+perp.y*2*single.NODE_SIZE);
    line[3].color = sf::Color::Black;

    if (arrow != 0) {
        sf::Vertex arr[3];

        float sign = 2 * arrow - 3;

        arr[0].position = line[arrow-1].position + perp * (single.ARROW_THICKNESS+1) * single.NODE_SIZE;
        arr[1].position = line[arrow-1].position - perp * (single.ARROW_THICKNESS-1) * single.NODE_SIZE;
        arr[2].position = sf::Vector2f(sign*(cX*0.5f/mag)*single.NODE_SIZE+line[arrow-1].position.x + perp.x*single.NODE_SIZE, sign*(cY*0.5f/mag)*single.NODE_SIZE+line[arrow-1].position.y + perp.y*single.NODE_SIZE);

        arr[0].color = sf::Color::Black;
        arr[1].color = sf::Color::Black;
        arr[2].color = sf::Color::Black;

        single.window.draw(arr, 3, sf::Triangles);
    }

    single.window.draw(line, 4, sf::Quads);
}

void GraphState::toggleDirectedMode() {
    directed = !directed;
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

void GraphState::toggleConnectMode() {
    if (mode == Mode::Connect)
        mode = Single::instance().DEFAULT_MODE;
    else
        mode = Mode::Connect;
}

std::string GraphState::getMode() const {
    std::string ret;

    switch (mode) {
        case Edit:
            ret = "Edit";
            break;
        case Connect:
            ret = "Connect";
            break;
        case View:
            ret = "View";
            break;
        case Typing:
            ret = "Name";
            break;
    }

    if (adjMode)
        ret += " (Adjacency)";

    if (densityMode)
        ret += " (Density Map)";

    if (forceMode) {
        if (inverseForce)
            ret += " (Force - Attraction)";
        else {
            ret += " (Force - Repulsion)";
        }
    }

    return ret;
}

void GraphState::physicsUpdate() {
    Single& single = Single::instance();

    auto mag = [](sf::Vector2f vc) {
        return (float)sqrt(pow(vc.x, 2) + pow(vc.y, 2));
    };

    auto velVectorRep = [&single, this](Node* one, Node* two) {
        auto dist = distance(one, two)/single.NODE_SIZE;
        auto unitVector = sf::Vector2f(one->x - two->x, one->y - two->y) / dist;
        return unitVector*single.REP_CONST/(float)pow(fmax(dist, single.NODE_SIZE), 2);};

    auto velVectorSpring = [&single, this](Node* one, Node* two) {
        auto dist = distance(one, two)/single.NODE_SIZE;
        auto unitVector = sf::Vector2f(one->x - two->x, one->y - two->y) / dist;
        auto spring = unitVector*single.SPRING_CONST*(float)pow(dist - single.springRestLen, 2);
        float sign = ((dist < single.springRestLen) ? 1.0f : -1.0f);

        return spring*sign;
    };

    for (Node* node1 : nodes) {
        for (Node* cons : node1->connections) {
            node1->velocity += velVectorSpring(node1, cons);
            cons->velocity += velVectorSpring(cons, node1);

            if (single.SPRING_FRICTION >= mag(node1->velocity))
                node1->velocity = sf::Vector2f(0, 0);
            else
                node1->velocity -= (node1->velocity/mag(node1->velocity))*single.SPRING_FRICTION*single.spFrictionMult;

            if (single.SPRING_FRICTION >= mag(cons->velocity))
                cons->velocity = sf::Vector2f(0, 0);
            else
                cons->velocity -= (cons->velocity/mag(cons->velocity))*single.SPRING_FRICTION*single.spFrictionMult;
        }

        for (Node* node2 : nodes) {
            if (node1 != node2) {
                node1->velocity += velVectorRep(node1, node2) * ((inverseForce) ? -1.f : 1.f);
            }
        }

        for (Node* node2 : nodes) {
            if (node1 != node2) {
                auto dist = distance(node1, node2);

                if (dist <= single.NODE_SIZE) {
                    auto vel = node1->velocity;
                    sf::Vector2f distVec(node2->x - node1->x, node2->y - node1->y);
                    auto unit = distVec/dist;

                    /*auto rePos = distVec-unit;

                    //node1->x += rePos.x;
                    //node1->y += rePos.y;

                    //node1->velocity += velVectorRep(node1, node2) / std::fmax(dist, 0.125f);*/

                    if ((vel.x * unit.x) + (vel.y * unit.y) >= 0) {
                        auto Va = node1->velocity - node2->velocity;
                        float scale = Va.x*unit.x + Va.y*unit.y;

                        sf::Vector2f Vaf = {Va.x - scale*unit.x, Va.y - scale*unit.y};

                        node1->velocity = Vaf + node2->velocity;
                        node2->velocity += scale * unit;
                    }

                    //node1->velocity /= (single.NORM_FRICTION - 1) * single.frictionMult + 1;
                    //node2->velocity /= ((single.NORM_FRICTION - 1) * single.frictionMult + 1);
                }
            }
        }
    }

    for (Node* node : nodes) {
        if (node->locked) {
            node->velocity = {0, 0};
        }

        auto v = node->velocity;
        float mag = sqrt(pow(v.x, 2)+pow(v.y, 2));

        if (mag == 0)
            continue;

        v *= single.NORM_FRICTION * single.frictionMult / mag;

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

float GraphState::distance(Node *one, Node *two) {
    return sqrt(pow(one->x-two->x, 2)+pow(one->y-two->y, 2));
}

void GraphState::toggleForce() {
    forceMode = !forceMode;

    if (!forceMode) {
        for (Node* n : nodes)
            n->velocity = sf::Vector2f(0, 0);
    }
}

std::vector<Node *> GraphState::getNodes() {
    return nodes;
}

int GraphState::getNodeIndex(Node *node) {
    for (int i = 0; i < nodes.size(); i++) {
        if (node == nodes[i])
            return i + 1;
    }

    return -1;
}

void GraphState::toggleDensity() {
    densityMode = !densityMode;
}

void GraphState::drawDensityMap() {
    int index = 0;
    float weight, dst;

    Single& single = Single::instance();

    auto viewSize = single.defaultView.getSize();

    points.resize(viewSize.x * viewSize.y);

    int nWidth = viewSize.x;
    int nHeight = viewSize.y;

    for (int x = 0; x < nWidth; x++) {
        for (int y = 0; y < nHeight; y++) {
            sf::Vector2i pos(x, y);

            weight = 0;

            for (Node* n : nodes) {
                dst = distanceSq(n, pos);

                if (dst <= 0.25)
                    continue;

                weight += 1 / dst;
            }

            points[index].position = sf::Vector2f(x, y);
            points[index++].color = gradient(weight);
        }
    }

    Single::instance().window.draw(points);
}

void GraphState::densityThreadCaller() {
    Single &single = Single::instance();

    auto size = single.window.getSize();

    single.state->points.resize(size.x * size.y);

    int segments = Single::instance().THREADS;

    int totalWidth = size.x;
    int nHeight = size.y;

    auto drawDensity = [&single, &nHeight, &totalWidth](int segment, int maxSegments) {
        float weight, dst;
        int index = segment;

        for (int x = segment; x < totalWidth; x += maxSegments) {
            for (int y = 0; y < nHeight; y++) {
                sf::Vector2i pos(x, y);

                weight = 0;

                for (Node* n : single.state->getNodes()) {
                    dst = single.state->distanceSq(n, pos);

                    if (dst <= 0.25*single.NODE_SIZE*single.NODE_SIZE)
                        continue;

                    weight += single.NODE_SIZE * single.NODE_SIZE / dst;
                }

                single.state->points[index].position = sf::Vector2f(x, y);
                single.state->points[index].color = single.state->gradient(weight);

                index += maxSegments;
            }
        }
    };

    std::thread threads[segments];

    for (int i = 0; i < segments; i++)
        threads[i] = std::thread(drawDensity, i, segments);

    for (int i = 0; i < segments; i++)
        threads[i].join();

    Single::instance().window.draw(Single::instance().state->points);
}

float GraphState::distanceSq(Node* &one, sf::Vector2i& two) {
    Single& single = Single::instance();

    auto pos = single.window.mapPixelToCoords(two);

    float dif1 = one->x-pos.x;
    float dif2 = one->y-pos.y;

    return (dif1*dif1)+(dif2*dif2);
}

sf::Color GraphState::gradient(float weight) {
    Single &single = Single::instance();

    //float alpha = pow(weight, 0.75)*255/2.0f;
    float alpha = weight * 85;

    sf::Vector3i &oneV = single.gradOne;
    sf::Vector3i &twoV = single.gradTwo;

    if (alpha <= 255)
        return {static_cast<sf::Uint8>(oneV.x), static_cast<sf::Uint8>(oneV.y), static_cast<sf::Uint8>(oneV.z), static_cast<sf::Uint8>(fmin(alpha,255))};
    else if (alpha <= 510) {
        sf::Vector3i delta1 = twoV - oneV;

        alpha -= 255;

        return {static_cast<sf::Uint8>(oneV.x + alpha / 255.0f * delta1.x),
                static_cast<sf::Uint8>(oneV.y + alpha / 255.0f * delta1.y),
                static_cast<sf::Uint8>(oneV.z + alpha / 255.0f * delta1.z)};
    }
    else if (alpha <= 765) {
        alpha -= 510;

        sf::Vector3i &threeV = single.gradThree;

        sf::Vector3i delta2 = threeV - twoV;

        return {static_cast<sf::Uint8>(twoV.x + alpha / 255.0f * delta2.x),
                static_cast<sf::Uint8>(twoV.y + alpha / 255.0f * delta2.y),
                static_cast<sf::Uint8>(twoV.z + alpha / 255.0f * delta2.z)};
    }

    return sf::Color::Red;
}

void GraphState::invertForce() {
    inverseForce = !inverseForce;
}

void GraphState::toggleOptMode() {
    optMode = !optMode;
}

void GraphState::toggleAdjMode() {
    adjMode = !adjMode;
}

bool GraphState::adjacentTo(const Node *pNode, const Node *mainNode) {
    if (pNode == mainNode)
        return false;

    for (Node* subNodes : mainNode->connections)
        if (subNodes == pNode)
            return true;

    if (!directed)
        for (Node* subNodes : pNode->connections)
            if (subNodes == mainNode)
                return true;

    return false;
}

std::string GraphState::getButtonAtPoint(sf::Vector2i pos) {
    Single& single = Single::instance();

    auto rect = single.window.getViewport(single.toolView);

    if (!rect.contains(pos))
        return "E~ButtonNotFound";

    int width = rect.width / single.TOOL_BUTTONS.size();

    return single.TOOL_BUTTONS[((int) (pos.x - ((int) pos.x % width) + 0.1f))/width];
}

void GraphState::reset() {
    Single& single = Single::instance();

    for (Node* n : nodes)
        delete n;

    selectedNode = nullptr;

    nodes.clear();

    single.defaultView.reset(sf::FloatRect(0, 0, single.WIDTH, single.HEIGHT));
    single.defaultView.setViewport(sf::FloatRect(0.0f, (float)single.TOOL_HEIGHT / single.HEIGHT, 1.0f, 1.0f));

    mode = Mode::Edit;

    adjMode = false;
    directed = false;
    forceMode = false;
    inverseForce = false;

    single.window.setTitle("Untitled - GraphBox");
    single.fileName = "";
}

void GraphState::saveFile(std::string filename) {
    Single& single = Single::instance();

    std::ofstream file(filename);

    int nodeNum = nodeCount();

    file << nodeNum << "|";

    auto nodeIndex = [this, nodeNum](Node* node) {
        for (int i = 0; i < nodeNum; i++) {
            if (node == nodes[i])
                return i;
        }

        return -1;
    };

    for (int i = 0; i < nodeNum; i++) {
        auto node = nodes[i];
        int connections = node->connections.size();

        file << node->label << "|";
        file << node->x << "|" << node->y << "|";
        file << connections << "|";

        for (int j = 0; j < connections; j++)
            file << nodeIndex(node->connections[j]) << "|";
    }

    if (directed)
        file << "1|";
    else
        file << "0|";

    file.close();
}

void GraphState::loadFile(std::string filename) {
    Single& single = Single::instance();

    std::ifstream file(filename);
    std::string data;

    std::getline(file, data);

    reset();

    auto pullData = [&data]() {
        int prevDem = data.find('|');
        std::string token = data.substr(0, prevDem);

        if (prevDem != data.size() - 1)
            data = data.substr(prevDem+1);

        return token;
    };

    int nodeCount = std::stoi(pullData());

    for (int i = 0; i < nodeCount; i++) {
        nodes.push_back(new Node());
    }

    for (int i = 0; i < nodeCount; i++) {
        Node* node = nodes[i];

        std::string label = pullData();

        float x = std::stof(pullData());
        float y = std::stof(pullData());

        int connections = std::stoi(pullData());

        node->label = label;
        node->x = x;
        node->y = y;

        for (int k = 0; k < connections; k++)
            node->connections.push_back(nodes[std::stoi(pullData())]);
    }

    directed = pullData() == "1";

    file.close();
}
