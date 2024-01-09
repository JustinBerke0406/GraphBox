#include "Slider.h"
#include "../Single.h"

void Slider::updateValueDependents() {
    slider.setSize(sf::Vector2f(width * ((float) value/abs(maxValue - minValue)), height));

    text.setString(std::to_string((int) round(value)));
    text.setOrigin(0, text.getLocalBounds().height/2);
    text.setPosition(xCord + width + 8, yCord + height/2 - 5);
}

void Slider::draw(RenderWindow& window) {
    Single& single = Single::instance();

    axis = sf::RectangleShape(sf::Vector2f(width, height));
    axis.setPosition(xCord, yCord);
    axis.setFillColor(sf::Color(200, 200, 200));

    slider = sf::RectangleShape(sf::Vector2f(width, height));
    slider.setPosition(xCord, yCord);
    slider.setFillColor(sf::Color::Black);

    text = sf::Text();
    text.setFont(single.font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);

    onInteract(window);

    window.draw(axis);
    window.draw(slider);
    window.draw(text);
}

void Slider::onInteract(RenderWindow& window) {
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), Single::instance().opView);

    if (isMouseClickHeld(window)) {
        int diff = mousePos.x - xCord;

        setValue(abs(maxValue - minValue) * (float) diff/ width);
    } else {
        updateValueDependents();
    }
}

bool Slider::checkBounds(sf::RenderWindow &window) {
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), Single::instance().opView);
    auto bounds = axis.getGlobalBounds();

    bounds.width += 1;

    return bounds.contains(mousePos.x, mousePos.y);
}
