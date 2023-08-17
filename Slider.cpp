#include "Slider.h"

Slider::Slider(int x, int y, float min, float max, float defaultValue, int width, int height) {
    this->xCord = x;
    this->yCord = y;
    this->minValue = min;
    this->maxValue = max;
    this->value = defaultValue;
    this->width = width;
    this->height = height;

    initGraphics();
}

float Slider::getRange() const {
    return abs(maxValue - minValue);
}

void Slider::setValue(float value) {
    this->value = value;

    updateValueDependents();
}

float Slider::getValue() const {
    return value;
}

void Slider::initGraphics() {
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

    updateValueDependents();
}

void Slider::updateValueDependents() {
    slider.setSize(sf::Vector2f(width * ((float) value/getRange()), height));

    text.setString(std::to_string((int) round(value)));
    text.setOrigin(0, text.getLocalBounds().height/2);
    text.setPosition(xCord + width + 8, yCord + height/2 - 5);
}

void Slider::draw(RenderWindow& window) {
    onInteract(window);

    window.draw(axis);
    window.draw(slider);
    window.draw(text);
}

void Slider::onInteract(RenderWindow& window) {
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), Single::instance().defaultView);
    auto bounds = axis.getGlobalBounds();

    bounds.width += 1;

    if (bounds.contains(mousePos.x, mousePos.y) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        int diff = mousePos.x - xCord;

        setValue(100 * (float) diff/ width);
    }
}
