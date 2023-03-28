#include <iostream>
#include "GraphBox.h"
#include "Single.h"

int launch() {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

    // Event needed for window to work
    sf::Event event{};

    // For loop to keep window open
    while (window.isOpen())
    {
        // Draw spriteHidden
        window.clear(sf::Color(255,255,255));

        // Render
        render();

        // draw method
        window.display();

        // Movement and zooming
        registerMovement();

        // Close window if the close event was triggered
        if (window.pollEvent(event)) {
            // Close window event
            if (event.type == sf::Event::Closed)
                window.close();
                // Mouse clicked event
            else if (event.type == sf::Event::MouseButtonPressed) {
                single.state->createNode(std::to_string(single.state->nodeCount()), event.mouseButton);
            }
            else if (event.type == sf::Event::KeyPressed) {

            }
            else if (event.type == sf::Event::Resized) {
                single.state->getPositionGrid().updateResizeData(event.size);
            }
        }
    }

    return 0;
}

void render() {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

    sf::CircleShape shape(1);

    shape.setFillColor(sf::Color(100, 250, 50));
    shape.setPosition(3, 3);

    single.state->getPositionGrid().transform(shape);

    window.draw(shape);

    sf::CircleShape shape1(1);

    shape1.setFillColor(sf::Color(100, 250, 50));
    shape1.setPosition(5, 3);

    single.state->getPositionGrid().transform(shape1);

    window.draw(shape1);

    single.state->drawNodes();
}

void registerMovement() {
    Single& single = Single::instance();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen)) {
        single.state->getPositionGrid().zoom(1.0f/single.ZOOM_SPEED);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal)) {
        single.state->getPositionGrid().zoom(single.ZOOM_SPEED);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        single.state->getPositionGrid().panRelative(-single.MOVE_SPEED, 0);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        single.state->getPositionGrid().panRelative(single.MOVE_SPEED, 0);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        single.state->getPositionGrid().panRelative(0, -single.MOVE_SPEED);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        single.state->getPositionGrid().panRelative(0, single.MOVE_SPEED);
    }
}

int main() {
    return launch();
}