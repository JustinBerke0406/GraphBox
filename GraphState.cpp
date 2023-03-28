#include "GraphState.h"

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

GraphState::GraphState() {

}

GraphState::PositionGrid &GraphState::getPositionGrid() {
    return positionGrid;
}
