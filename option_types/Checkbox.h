#pragma once

#include "OptionType.h"

class Checkbox : public OptionType<bool> {
    sf::RectangleShape box;

    float size;

    bool mousePrevClicked = false;

    void updateValueDependents() override;
    void onInteract(sf::RenderWindow& window) override;
protected:
    bool checkBounds(sf::RenderWindow& window) override;
public:
    Checkbox(int x, int y, bool defaultValue = false, float size = 35.0f): OptionType<bool>(x, y, defaultValue), size(size) {};

    void draw(sf::RenderWindow& window) override;
};
