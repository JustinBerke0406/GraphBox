#pragma once

#include "../Single.h"

class ViewHelper {
public:
    static std::string getToolboxButtonAtPoint(sf::Vector2i pos);

    static bool cursorOverClickable();
};