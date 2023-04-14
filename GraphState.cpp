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

        auto mPos = getPositionGrid().gl_loc(sf::Mouse::getPosition(single.window));
        /*

        float cX = mPos[0] - selectedNode->x,
              cY = mPos[1] - selectedNode->y;

        float mag = sqrt(pow(cX, 2) + pow(cY, 2));

        sf::Vector2f perp(-cY*single.EDGE_THICKNESS/mag, cX*single.EDGE_THICKNESS/mag);

        line[0].position = sf::Vector2f(mPos[0] - perp.x, mPos[1] - perp.y);
        line[0].color = sf::Color::Black;

        line[1].position = sf::Vector2f((cX*0.5f/mag)+selectedNode->x - perp.x, (cY*0.5f/mag)+selectedNode->y - perp.y);
        line[1].color = sf::Color::Black;

        line[2].position = sf::Vector2f(line[1].position.x+perp.x*2, line[1].position.y+perp.y*2);
        line[2].color = sf::Color::Black;

        line[3].position = sf::Vector2f(line[0].position.x+perp.x*2, line[0].position.y+perp.y*2);
        line[3].color = sf::Color::Black;

        getPositionGrid().transform(line[0], line[1]);
        getPositionGrid().transform(line[2], line[3]);

        single.window.draw(line, 4, sf::Quads);*/

        drawEdge(sf::Vector2f(selectedNode->x, selectedNode->y), sf::Vector2f(mPos[0], mPos[1]), true, false, 0);
    }
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
