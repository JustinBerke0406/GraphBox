#include <iostream>
#include "GraphBox.h"
#include "Single.h"

int launch() {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

    // Event needed for window to work
    sf::Event event{};

    sf::Cursor cursor;

    bool defaultCursor = true;
    bool mouseHeld = false;
    bool offsetSet = false;

    int timeMouseHeld = 0;

    sf::Vector2i offset;
    Node* draggedNode = nullptr;

    // For loop to keep window open
    while (window.isOpen())
    {
        // Draw spriteHidden
        window.clear(sf::Color(255,255,255));

        // Render
        render();

        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            timeMouseHeld = 0;
            offsetSet = false;
            draggedNode = nullptr;
        }

        if (mouseHeld) {
            timeMouseHeld++;

            if (timeMouseHeld >= single.MOUSE_HOLD_TIME) {
                auto mousePos = sf::Mouse::getPosition(single.window);

                Node* node = single.state->nodeAt(mousePos);

                if (node != nullptr || draggedNode != nullptr) {
                    if (draggedNode == nullptr)
                        draggedNode = node;

                    if (!offsetSet) {
                        auto globalNode = single.state->getPositionGrid().loc_gl(draggedNode->x, draggedNode->y);
                        offset = sf::Vector2i(mousePos.x - globalNode[0], mousePos.y - globalNode[1]);
                    }

                    offsetSet = true;

                    onMouseHeld(offset, draggedNode);
                }
            }
        }

        // draw method
        window.display();

        // Movement and zooming
        if (single.state->mode != GraphState::Mode::Typing)
            registerMovement();

        if (single.state->cursorOverClickable() && defaultCursor) {
            cursor.loadFromSystem(sf::Cursor::Hand);
            window.setMouseCursor(cursor);

            defaultCursor = false;
        }
        else if (!single.state->cursorOverClickable() && !defaultCursor) {
            cursor.loadFromSystem(sf::Cursor::Arrow);
            window.setMouseCursor(cursor);

            defaultCursor = true;
        }

        // Close window if the close event was triggered
        if (window.pollEvent(event)) {
            // Close window event
            if (event.type == sf::Event::Closed)
                window.close();
                // Mouse clicked event
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (single.state->mode != GraphState::Mode::View) {
                    if (single.state->mode != GraphState::Mode::Typing) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            mouseHeld = false;

                            if (single.state->mode == GraphState::Mode::Edit &&
                                !single.state->toggleNode(event.mouseButton))
                                single.state->createNode(event.mouseButton);
                            else if (single.state->mode == GraphState::Mode::Connect) {
                                if (single.state->wouldSelect(event.mouseButton))
                                    single.state->addConnection(single.state->nodeAt(event.mouseButton));

                                single.state->mode = GraphState::Mode::Edit;
                            }
                            else if (single.state->mode == GraphState::Mode::Edit) {
                                mouseHeld = true;
                            }
                        }
                        else if (event.mouseButton.button == sf::Mouse::Right &&
                                 single.state->mode == GraphState::Mode::Edit)
                            single.state->deleteNode(single.state->nodeAt(event.mouseButton));
                    }
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (single.state->mode != GraphState::Mode::Typing) {
                    if (event.key.code == sf::Keyboard::C)
                        single.state->mode = GraphState::Mode::Connect;
                    else if (event.key.code == sf::Keyboard::D)
                        single.state->toggleDirectedMode();
                }
            }
            else if (event.type == sf::Event::Resized) {
                single.state->getPositionGrid().updateResizeData(event.size);
            }
        }
    }

    return 0;
}

void onMouseHeld(sf::Vector2i offset, Node* node) {
    Single& single = Single::instance();

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        single.state->changeNodePosition(node, sf::Mouse::getPosition(single.window) - offset);
    }
}

void render() {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

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