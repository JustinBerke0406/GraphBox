//
// Created by Justin on 8/18/2023.
//

#include "ViewHelper.h"

std::string ViewHelper::getToolboxButtonAtPoint(sf::Vector2i pos) {
    Single& single = Single::instance();

    auto rect = single.window.getViewport(single.toolView);

    if (!rect.contains(pos))
        return "E~ButtonNotFound";

    int width = rect.width / single.TOOL_BUTTONS.size();

    return single.TOOL_BUTTONS[((int) (pos.x - ((int) pos.x % width) + 0.1f))/width];
}

bool ViewHelper::cursorOverClickable() {
    Single& single = Single::instance();

    GraphState* state = single.state;

    auto mousePos = sf::Mouse::getPosition(single.window);

    if (single.window.getViewport(single.defaultView).contains(mousePos)) {
        Node *node = state->nodeAt(mousePos);

        if (node == nullptr)
            return false;

        return true;
    }
    else if (single.window.getViewport(single.toolView).contains(mousePos)) {
        return true;
    }

    return false;
}
