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
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), Single::instance().opView);

    bool testCond = box.getGlobalBounds().contains(mousePos.x, mousePos.y) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (testCond && !mousePrevClicked) {
        setValue(!value);
        mousePrevClicked = true;
    } else if (!testCond){
        mousePrevClicked = false;
    }

    updateValueDependents();
}
