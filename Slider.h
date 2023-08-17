#pragma once

#include "Single.h"

class Slider {
private:
    sf::RectangleShape slider;
    sf::RectangleShape axis;
    sf::Text text;
    float minValue;
    float maxValue;
    int xCord;
    int yCord;
    int width;
    int height;
    float value;

    float getRange() const;

    void initGraphics();

    void updateValueDependents();

    void onInteract(sf::RenderWindow& window);
public:
    Slider(int x, int y, float min, float max, float defaultValue = 0.0f, int width = 150, int height = 25);

    void setValue(float value);
    float getValue() const;

    void draw(sf::RenderWindow& window);
};