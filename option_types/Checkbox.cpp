#include <SFML/Graphics/RectangleShape.hpp>
#include "Checkbox.h"
#include "../Single.h"

void Checkbox::draw(sf::RenderWindow &window) {
    box = sf::RectangleShape(sf::Vector2f(size, size));
    box.setPosition(xCord, yCord);

    box.setOutlineThickness(-2);
    box.setOutlineColor(sf::Color::Black);

    onInteract(window);

    window.draw(box);
}

void Checkbox::updateValueDependents() {
    if (value)
        box.setFillColor(sf::Color::Blue);
    else
        box.setFillColor(sf::Color::White);
}

void Checkbox::onInteract(sf::RenderWindow& window) {
    bool testCond = isMouseClickHeld(window);

    if (testCond && !mousePrevClicked) {
        setValue(!value);
        mousePrevClicked = true;
    } else if (!testCond){
        mousePrevClicked = false;
    }

    updateValueDependents();
}

bool Checkbox::checkBounds(RenderWindow &window) {
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), Single::instance().opView);

    return box.getGlobalBounds().contains(mousePos.x, mousePos.y);
}
