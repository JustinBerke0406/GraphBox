#include "GraphBox.h"

int launch() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "GraphBox");

    // Event needed for window to work
    sf::Event event{};

    // For loop to keep window open
    while (window.isOpen())
    {
        // Draw spriteHidden
        window.clear();
        // draw method
        window.display();

        // Close window if the close event was triggered
        if (window.pollEvent(event) and event.type == sf::Event::Closed)
            window.close();
    }

    return 0;
}

int main() {
    return launch();
}