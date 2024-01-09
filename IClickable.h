#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include "helper/InputHelper.h"

class IClickable {
protected:
    Time entered;
    Time left;

    virtual bool checkBounds(RenderWindow& window) = 0;

    void onEnter() {
        InputHelper& inputHelper = InputHelper::instance();

        entered = inputHelper.elapsedTime();
    }

    void onLeave() {
        InputHelper& inputHelper = InputHelper::instance();

        left = inputHelper.elapsedTime();
    }

    bool isMouseHeld(RenderWindow& window) {
        return Mouse::isButtonPressed(Mouse::Left) && isMouseOver(window);
    }

    bool isMouseClickHeld(RenderWindow& window) {
        InputHelper& inputHelper = InputHelper::instance();

        return isMouseHeld(window) && inputHelper.pressedAt(Mouse::Left) >= entered;
    }
public:
    bool isMouseOver(RenderWindow& window) {
        bool bounded = checkBounds(window);

        if (bounded && left >= entered)
            onEnter();
        else if (!bounded && left <= entered)
            onLeave();

        return bounded;
    }
};