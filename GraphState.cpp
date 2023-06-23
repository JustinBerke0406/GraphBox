#include "GraphState.h"

#include <cmath>
#include <utility>
#include <iostream>
#include "Single.h"

void GraphState::PositionGrid::zoom(float scale, float x, float y) {
    float scaleChange = (scale - 1)/scale;

    xOrigin += scaleChange*(x-xOrigin);
    yOrigin += scaleChange*(y-yOrigin);

    zoomScale *= scale;
}

void GraphState::PositionGrid::zoom(float scale, sf::Event::MouseWheelScrollEvent buttonEvent) {
    std::array<float, 2> newPos = gl_loc(buttonEvent.x, buttonEvent.y);

    zoom(scale, newPos[0], newPos[1]);
}

void GraphState::PositionGrid::zoom(float scale) {
    auto pos = gl_loc(xResize * (float) WIDTH/2, yResize * (float) HEIGHT/2);

    zoom(scale, pos[0], pos[1]);
}

std::array<float, 2> GraphState::PositionGrid::gl_loc(int x, int y) const {
    std::array<float, 2> pos {
        (float) x / (zoomScale * xResize) + xOrigin,
        (float) y / (zoomScale * yResize) + yOrigin};

    return pos;
}

std::array<float, 2> GraphState::PositionGrid::gl_loc(sf::Event::MouseButtonEvent buttonEvent) const {
    std::array<float, 2> newPos = gl_loc(buttonEvent.x, buttonEvent.y);

    return newPos;
}

std::array<float, 2> GraphState::PositionGrid::gl_loc(sf::Vector2<int> &pos) const {
    return gl_loc(pos.x, pos.y);
}

std::array<int, 2> GraphState::PositionGrid::loc_gl(float x, float y) const {
    std::array<int, 2> pos {
            (int) round((double) ((x - xOrigin) * zoomScale)),
            (int) round((double) ((y - yOrigin) * zoomScale))};

    return pos;
}

void GraphState::PositionGrid::transform(sf::Transformable &shape) {
    shape.scale(zoomScale, zoomScale);

    std::array<int, 2> org = loc_gl(shape.getPosition().x, shape.getPosition().y);

    shape.setPosition(org[0], org[1]);
}

void GraphState::PositionGrid::transform(sf::CircleShape &shape) {
    shape.scale(zoomScale, zoomScale);

    std::array<int, 2> org = loc_gl(shape.getPosition().x, shape.getPosition().y);

    shape.setPosition(org[0], org[1]);

    shape.setPointCount(fmin(30 * fmax(zoomScale / 100, 2), 480));
}

void GraphState::PositionGrid::transform(sf::Text &shape) {
    shape.scale(zoomScale / 100, zoomScale / 100);

    std::array<int, 2> org = loc_gl(shape.getPosition().x, shape.getPosition().y);

    shape.setPosition(org[0], org[1]);
}

void GraphState::PositionGrid::transform(sf::Vertex* ones, int count) {
    for (int i = 0; i < count; i++) {
        auto& one = ones[i];
        auto glPos1 = loc_gl(one.position.x, one.position.y);

        one.position = sf::Vector2f(glPos1[0], glPos1[1]);
    }
}

GraphState::PositionGrid::PositionGrid() {
    xOrigin = 0.0f;
    yOrigin = 0.0f;

    zoomScale = 100.0f;

    xResize = 1.0f;
    yResize = 1.0f;
}

void GraphState::PositionGrid::updateResizeData(sf::Event::SizeEvent size) {
    xResize = (float) size.width / (float) WIDTH;
    yResize = (float) size.height / (float) HEIGHT;
}

void GraphState::PositionGrid::pan(float x, float y) {
    xOrigin += x;
    yOrigin += y;
}

void GraphState::PositionGrid::panRelative(float x, float y) {
    xOrigin += x / (zoomScale / 100);
    yOrigin += y / (zoomScale / 100);
}

std::array<sf::Vector2f, 2> GraphState::PositionGrid::getCorners() {
    sf::Vector2f org(xOrigin, yOrigin);

    auto pos = gl_loc(WIDTH, HEIGHT);

    sf::Vector2f bottom(pos[0], pos[1]);

    return std::array<sf::Vector2f, 2> {org, bottom};
}

std::array<float, 2> GraphState::PositionGrid::getResize() const {
    return std::array<float, 2> {xResize, yResize};
}

GraphState::PositionGrid &GraphState::getPositionGrid() {
    return positionGrid;
}

std::string GraphState::createNode(std::string label, float x, float y) {
    Node* node = new Node();
    node->x = x;
    node->y = y;
    node->label = std::move(label);

    nodes.push_back(node);

    return label;
}

std::string GraphState::createNode(std::string label, sf::Event::MouseButtonEvent event) {
    auto pos = positionGrid.gl_loc(event);

    return createNode(std::move(label), pos[0], pos[1]);
}

std::string GraphState::createNode(sf::Event::MouseButtonEvent event) {
    for (int i = 0; i < nodes.size(); i++)
        if (!isLabelTaken(std::to_string(i)))
            return createNode(std::to_string(i), event);

    return createNode(std::to_string(nodes.size()), event);
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
        circ.setRadius(0.5f);
        circ.setOrigin(circ.getLocalBounds().width/2, circ.getLocalBounds().height/2);
        circ.setOutlineThickness(-0.02f);
        circ.setOutlineColor(sf::Color::Black);

        sf::Color fillColor = (n == selectedNode) ? ((!errorLabel) ? single.HIGHLIGHT_COLOR : single.ERROR_COLOR) : single.NODE_COLOR;

        if (adjMode && selectedNode != nullptr) {
            if (adjacentTo(n, selectedNode))
                fillColor = single.ADJ_COLOR;
        }

        circ.setFillColor(fillColor);
        circ.setPosition(n->x, n->y);

        single.state->getPositionGrid().transform(circ);
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

        single.state->getPositionGrid().transform(text);
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

        auto mPos = getPositionGrid().gl_loc(mousePos);

        drawEdge(sf::Vector2f(selectedNode->x, selectedNode->y), sf::Vector2f(mPos[0], mPos[1]), true, false, 0);
    }

    sf::Text text;
    text.setFont(single.font);
    text.setCharacterSize(single.WINDOW_TEXT_SIZE);
    text.setFillColor(sf::Color::Black);
    text.setPosition(single.WINDOW_TEXT_X_OFF, single.WINDOW_TEXT_Y_OFF);
    text.setString("Mode: " + getMode());

    single.window.draw(text);
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
}

bool GraphState::cursorOverClickable() {
    Node* node = nodeAt(sf::Mouse::getPosition(Single::instance().window));

    if (node == nullptr)
        return false;

    return true;
}

Node* GraphState::nodeAt(sf::Event::MouseButtonEvent event) {
    auto pos = positionGrid.gl_loc(event);

    for (int i = nodes.size() - 1; i >= 0; i--) {
        Node& n = *nodes[i];

        if (sqrt(pow(pos[0] - n.x, 2)+pow(pos[1] - n.y, 2)) <= 0.5)
            return nodes[i];
    }

    return nullptr;
}

Node* GraphState::nodeAt(sf::Vector2<int> pos) {
    auto post = positionGrid.gl_loc(pos.x, pos.y);

    for (int i = nodes.size() - 1; i >= 0; i--) {
        Node& n = *nodes[i];

        if (sqrt(pow(post[0] - n.x, 2)+pow(post[1] - n.y, 2)) <= 0.5)
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

    line[0].position = sf::Vector2f((cX*0.5f/mag)*n1+mpos1.x - perp.x, (cY*0.5f/mag)*n1 + mpos1.y - perp.y);
    line[0].color = sf::Color::Black;

    line[1].position = sf::Vector2f(-(cX*0.5f/mag)*n2+mpos2.x - perp.x, -(cY*0.5f/mag)*n2+mpos2.y - perp.y);
    line[1].color = sf::Color::Black;

    line[2].position = sf::Vector2f(line[1].position.x+perp.x*2, line[1].position.y+perp.y*2);
    line[2].color = sf::Color::Black;

    line[3].position = sf::Vector2f(line[0].position.x+perp.x*2, line[0].position.y+perp.y*2);
    line[3].color = sf::Color::Black;

    if (arrow != 0) {
        sf::Vertex arr[3];

        int sign = -(3 - 2 * arrow);

        arr[0].position = sf::Vector2f(line[arrow-1].position.x + perp.x*(single.ARROW_THICKNESS+1), line[arrow-1].position.y + perp.y*(single.ARROW_THICKNESS+1));
        arr[1].position = sf::Vector2f(line[arrow-1].position.x - perp.x*(single.ARROW_THICKNESS-1), line[arrow-1].position.y - perp.y*(single.ARROW_THICKNESS-1));
        arr[2].position = sf::Vector2f(sign*(cX*0.5f/mag)+line[arrow-1].position.x + perp.x, sign*(cY*0.5f/mag)+line[arrow-1].position.y + perp.y);

        arr[0].color = sf::Color::Black;
        arr[1].color = sf::Color::Black;
        arr[2].color = sf::Color::Black;

        getPositionGrid().transform(arr, 3);

        single.window.draw(arr, 3, sf::Triangles);
    }

    getPositionGrid().transform(line, 4);

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
    auto pos = getPositionGrid().gl_loc(event);

    node->x = pos[0];
    node->y = pos[1];
}

void GraphState::changeNodePositionLocally(Node *node, sf::Vector2f pos) {
    node->x = pos.x;
    node->y = pos.y;
}

void GraphState::changeNodePosition(Node *node, sf::Vector2i mousePos) {
    auto pos = getPositionGrid().gl_loc(mousePos);

    node->x = pos[0];
    node->y = pos[1];
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
        auto dist = distance(one, two);
        auto unitVector = sf::Vector2f(one->x - two->x, one->y - two->y) / dist;
        return unitVector*single.REP_CONST/(float)pow(fmax(dist, 1), 2);};

    auto velVectorSpring = [&single, this, mag](Node* one, Node* two) {
        sf::Vector2f unit(two->x - one->x, two->y - one->y);
        unit /= mag(unit);
        sf::Vector2f spring = unit*single.SPRING_CONST*((float)pow(fmax(distance(one, two), 1)- single.SPRING_REST_LEN, 2))/(float)fmax(distance(one, two), 1);
        float sign = ((fmax(distance(one, two), 1) > single.SPRING_REST_LEN) ? 1.0f : -1.0f);

        return spring*sign;
    };

    for (Node* node1 : nodes) {
        for (Node* cons : node1->connections) {
            node1->velocity += velVectorSpring(node1, cons);
            cons->velocity += velVectorSpring(cons, node1);

            if (single.SPRING_FRICTION >= mag(node1->velocity))
                node1->velocity = sf::Vector2f(0, 0);
            else
                node1->velocity -= (node1->velocity/mag(node1->velocity))*single.SPRING_FRICTION;

            if (single.SPRING_FRICTION >= mag(cons->velocity))
                cons->velocity = sf::Vector2f(0, 0);
            else
                cons->velocity -= (cons->velocity/mag(cons->velocity))*single.SPRING_FRICTION;
        }

        for (Node* node2 : nodes) {
            if (node1 != node2) {
                node1->velocity += velVectorRep(node1, node2) * ((inverseForce) ? -1.f : 1.f);
            }
        }

        for (Node* node2 : nodes) {
            if (node1 != node2) {
                auto dist = distance(node1, node2);

                if (dist <= 1) {
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

                    node1->velocity *= single.NORM_FRICTION;
                    node2->velocity *= single.NORM_FRICTION;
                }
            }
        }
    }

    for (Node* node : nodes) {
        node->x += node->velocity.x * single.DELTA_TIME;
        node->y += node->velocity.y * single.DELTA_TIME;
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

    auto resize = positionGrid.getResize();

    points.resize(positionGrid.WIDTH * positionGrid.HEIGHT * resize[0] * resize[1]);

    int nWidth = positionGrid.WIDTH * resize[0];
    int nHeight = positionGrid.HEIGHT * resize[1];

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

            points[index].position = sf::Vector2f(x / resize[0], y / resize[1]);
            points[index++].color = gradient(weight);
        }
    }

    Single::instance().window.draw(points);
}

void GraphState::densityThreadCaller() {
    Single &single = Single::instance();

    auto resize = single.state->getPositionGrid().getResize();

    float invResizeX = 1 / resize[0], invResizeY = 1 / resize[1];

    single.state->points.resize(single.state->getPositionGrid().WIDTH * single.state->getPositionGrid().HEIGHT * resize[0] * resize[1]);

    int segments = Single::instance().THREADS;

    int totalWidth = single.state->getPositionGrid().WIDTH * resize[0];
    int nHeight = single.state->getPositionGrid().HEIGHT * resize[1];

    auto drawDensity = [&single, &nHeight, &invResizeX, &invResizeY, &totalWidth](int segment, int maxSegments) {
        float weight, dst;
        int index = segment;

        for (int x = segment; x < totalWidth; x += maxSegments) {
            float xSize = x * invResizeX;

            for (int y = 0; y < nHeight; y++) {
                sf::Vector2i pos(x, y);

                weight = 0;

                for (Node* n : single.state->getNodes()) {
                    dst = single.state->distanceSq(n, pos);

                    if (dst <= 0.25)
                        continue;

                    weight += 1 / dst;
                }

                single.state->points[index].position = sf::Vector2f(xSize, y * invResizeY);
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
    auto pos = getPositionGrid().gl_loc(two);

    float dif1 = one->x-pos[0];
    float dif2 = one->y-pos[1];

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

void GraphState::initToolbox() {
    Single& single = Single::instance();

    single.window.setView(single.toolView);

    sf::RectangleShape box(single.toolView.getSize());
    box.setPosition(0, 0);
    box.setFillColor(sf::Color(220, 220, 220));
    box.setOutlineThickness(-2);
    box.setOutlineColor(sf::Color::Black);

    single.window.draw(box);

    single.window.setView(single.defaultView);
}
