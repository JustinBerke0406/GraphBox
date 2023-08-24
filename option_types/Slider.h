#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include "OptionType.h"

class Slider : public OptionType<float> {
private:
    sf::RectangleShape slider;
    sf::RectangleShape axis;
    sf::Text text;
    float minValue;
    float maxValue;
    int width;
    int height;

    void updateValueDependents() override;
    void onInteract(sf::RenderWindow& window) override;
public:
    Slider(int x, int y, float min, float max, float defaultValue = 0.0f, int width = 150, int height = 25): OptionType<float>(x, y, defaultValue),
            minValue(min), maxValue(max), width(width), height(height) {};

    void draw(sf::RenderWindow& window) override;
};