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

        // Close window if the close event was triggered
        if (window.pollEvent(event) and event.type == sf::Event::Closed)
            window.close();
    }

    return 0;
}

void render() {

}

int main() {
    return launch();
}