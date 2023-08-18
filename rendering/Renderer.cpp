#include "Renderer.h"
#include "ViewRenderer.h"

void Renderer::drawState() {
    Single& single = Single::instance();

    GraphState* state = single.state;

    std::vector<sf::CircleShape> circles;
    std::vector<sf::Text> texts;

    if (single.mode["type"]) {
        textBlinker++;

        if (textBlinker > single.TEXT_BLINKER_TIME*2)
            textBlinker = 0;
    }

    auto nodes = state->getNodes();
    auto selectedNode = state->getSelectedNode();

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

        sf::Color fillColor = (n == selectedNode) ? ((!single.mode["error"]) ? single.HIGHLIGHT_COLOR : single.ERROR_COLOR) : single.NODE_COLOR;

        if (single.mode["adj"] && selectedNode != nullptr) {
            if (state->adjacentTo(n, selectedNode))
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

        if (single.mode["type"] && textBlinker < single.TEXT_BLINKER_TIME && n == selectedNode && !n->label.empty()) {
            text.setString(n->label + "_");
        }

        texts.push_back(text);

        for (Node* con : n->connections)
            drawEdge(sf::Vector2f(n->x, n->y), sf::Vector2f(con->x, con->y), true, true, 2*single.mode["directed"]);
    }

    for (int i = 0; i < texts.size(); i++) {
        single.window.draw(circles[i]);
        single.window.draw(texts[i]);
    }

    if (single.mode.mainMode==ModeHandler::Mode::Connect
        && selectedNode != nullptr
        && state->nodeAt(sf::Mouse::getPosition(single.window)) != selectedNode) {

        auto mousePos = sf::Mouse::getPosition(single.window);

        auto mPos = single.window.mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y));

        drawEdge(sf::Vector2f(selectedNode->x, selectedNode->y), sf::Vector2f(mPos.x, mPos.y), true, false, 0);
    }
}

void Renderer::drawEdge(sf::Vector2f pos1, sf::Vector2f pos2, int n1, int n2, char arrow) {
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

void Renderer::draw() {
    Single& single = Single::instance();

    drawState();

    ViewRenderer::renderToolbar();

    if (single.mode["opt"])
        ViewRenderer::renderOptions();
}

void Renderer::operator()() {
    draw();
}
