#pragma once

#include <SFML/Graphics/Color.hpp>

struct Maths {
    static sf::Color lerp(float rate, sf::Color start, sf::Color end) {
        return {static_cast<sf::Uint8>(rate * (float) (end.r - start.r) + start.r),
                static_cast<sf::Uint8>(rate * (float) (end.g - start.g) + start.g),
                static_cast<sf::Uint8>(rate * (float) (end.b - start.b) + start.b),
                static_cast<sf::Uint8>(rate * (float) (end.a - start.a) + start.a)};
    }

    static sf::Color lerp(float rate, sf::Color start, sf::Color middle, sf::Color end) {
        return (rate <= 0.5) ? lerp(2 * rate, start, middle) : lerp(2 * rate - 1, middle, end);
    }

    static sf::Color lerp(float rate, std::vector<sf::Color> colors) {
        int n = colors.size() - 1;

        int interval = rate * (float) n - (int) rate;

        return lerp(n * rate - interval, colors[interval], colors[interval + 1]);
    }
};