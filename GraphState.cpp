#include "GraphState.h"

#include <utility>
#include "Single.h"

void GraphState::PositionGrid::zoom(float scale, float x, float y) {
    float scaleChange = (scale - 1)/scale;

    xOrigin += scaleChange*(x-xOrigin);
    yOrigin += scaleChange*(y-yOrigin);

    zoomScale *= scale;
}

void GraphState::PositionGrid::zoom(float scale, sf::Event::MouseButtonEvent buttonEvent) {
    std::array<float, 2> newPos = gl_loc(buttonEvent);

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

std::array<float, 2> GraphState::PositionGrid::gl_loc(sf::Vector2<int> pos) const {
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

    shape.setCharacterSize(shape.getCharacterSize());
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

GraphState::PositionGrid &GraphState::getPositionGrid() {
    return positionGrid;
}

bool GraphState::createNode(std::string label, float x, float y) {
    Node* node = new Node();
    node->x = x;
    node->y = y;
    node->label = std::move(label);

    if (std::any_of(nodes.begin(), nodes.end(), [node](Node* n) {return node->label == n->label;})) return false;

    nodes.push_back(node);

    return true;
}

bool GraphState::createNode(std::string label, sf::Event::MouseButtonEvent event) {
    auto pos = positionGrid.gl_loc(event);

    return createNode(label, pos[0], pos[1]);
}

bool GraphState::deleteNode(std::string &label) {
    for (int i = 0; i < nodes.size(); i++)
        if (nodes[i]->label == label) {
            nodes.erase(nodes.begin() + i);
            return true;
        }

    return false;
}

void GraphState::drawNodes() {
    Single& single = Single::instance();

    std::vector<sf::CircleShape> circles;
    std::vector<sf::Text> texts;

    for (const Node* n : nodes) {
        sf::CircleShape circ;
        circ.setRadius(0.5f);
        circ.setOrigin(circ.getLocalBounds().width/2, circ.getLocalBounds().height/2);
        circ.setOutlineThickness(0.02f);
        circ.setOutlineColor(sf::Color::Black);
        circ.setFillColor((n == selectedNode) ? single.HIGHLIGHT_COLOR : single.NODE_COLOR);
        circ.setPosition(n->x, n->y);

        single.state->getPositionGrid().transform(circ);
        circles.push_back(circ);

        sf::Text text;
        text.setFont(single.font);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::Black);
        text.setPosition(n->x, n->y);
        text.setString(n->label);
        single.state->getPositionGrid().transform(text);
        text.setOrigin(text.getLocalBounds().width/2, text.getLocalBounds().height);

        texts.push_back(text);

        //single.window.draw(text);
    }

    for (int i = 0; i < texts.size(); i++) {
        single.window.draw(circles[i]);
        single.window.draw(texts[i]);
    }
}

int GraphState::nodeCount() {
    return nodes.size();
}

bool GraphState::selectNode(sf::Event::MouseButtonEvent event) {
    auto pos = positionGrid.gl_loc(event);

    for (int i = nodes.size() - 1; i >= 0; i--) {
        Node& n = *nodes[i];

        if (sqrt(pow(pos[0] - n.x, 2)+pow(pos[1] - n.y, 2)) <= 0.5) {
            selectedNode = &n;
            return true;
        }
    }

    return false;
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

GraphState::~GraphState() {
    for (Node* n : nodes)
        delete n;

    nodes.clear();
}

bool GraphState::cursorOverClickable() {
    auto pos = positionGrid.gl_loc(sf::Mouse::getPosition(Single::instance().window));

    for (int i = nodes.size() - 1; i >= 0; i--) {
        Node& n = *nodes[i];

        if (sqrt(pow(pos[0] - n.x, 2)+pow(pos[1] - n.y, 2)) <= 0.5)
            return true;
    }

    return false;
}
