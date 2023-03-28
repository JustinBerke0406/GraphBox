#pragma once

#include <cmath>
#include <array>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class GraphState {
private:
    class PositionGrid {
    private:
        float xOrigin, yOrigin, zoomScale, xResize, yResize;
    public:
        PositionGrid();

        const int WIDTH = 1920, HEIGHT = 1080;

        void zoom(float scale);
        void zoom(float scale, float x, float y);
        void zoom(float scale, sf::Event::MouseButtonEvent buttonEvent);

        void pan(float x, float y);
        void panRelative(float x, float y);

        [[nodiscard]] std::array<float, 2> gl_loc(int x, int y) const;
        [[nodiscard]] std::array<float, 2> gl_loc(sf::Event::MouseButtonEvent buttonEvent) const;

        [[nodiscard]] std::array<int, 2> loc_gl(float x, float y) const;

        void transform(sf::Transformable& shape);
        void transform(sf::CircleShape& shape);

        void updateResizeData(sf::Event::SizeEvent size);
    };

    PositionGrid positionGrid;
public:
    GraphState();

    PositionGrid& getPositionGrid();
};