#include <iostream>
#include "GraphBox.h"
#include "Single.h"

int launch() {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

    window.setKeyRepeatEnabled(false);

    // Event needed for window to work
    sf::Event event{};

    sf::Cursor cursor;

    bool defaultCursor = true;
    bool mouseHeld = false;
    bool offsetSet = false;
    bool readyToSelect = false;
    bool readyDouble = false;
    bool activateDouble = false;

    int timeMouseHeld = 0;
    int doubleClickTimer = 0;

    std::string prevLabel;

    sf::Vector2f offset;
    Node* draggedNode = nullptr;

    // For loop to keep window open
    while (window.isOpen())
    {
        // Draw spriteHidden
        window.clear(sf::Color(255,255,255));

        // Render
        render();

        if (single.state->forceMode)
            single.state->physicsUpdate();

        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            timeMouseHeld = 0;
            offsetSet = false;
            draggedNode = nullptr;

            if (readyDouble) {
                doubleClickTimer++;

                if (activateDouble) {
                    if (doubleClickTimer < single.DOUBLE_CLICK_TIME) {
                        Node* node = single.state->nodeAt(sf::Mouse::getPosition(single.window));

                        single.state->selectNode(node);
                        single.state->mode = GraphState::Mode::Typing;

                        prevLabel = node->label;

                        readyToSelect = false;
                    }

                    readyDouble = false;
                    activateDouble = false;
                    doubleClickTimer = 0;
                }
            }

            if (readyToSelect) {
                single.state->toggleNode(single.state->nodeAt(sf::Mouse::getPosition(single.window)));
                readyToSelect = false;

                readyDouble = true;
            }
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
                        auto loMouse = single.state->getPositionGrid().gl_loc(mousePos);
                        auto glOff = single.state->getPositionGrid().loc_gl(loMouse[0] - draggedNode->x, loMouse[1] - draggedNode->y);

                        offset = sf::Vector2f(loMouse[0] - draggedNode->x, loMouse[1] - draggedNode->y);
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
                                !single.state->cursorOverClickable())
                                single.state->createNode(event.mouseButton);
                            else if (single.state->mode == GraphState::Mode::Connect) {
                                if (single.state->wouldSelect(event.mouseButton)) {
                                    if (!single.state->isNodeSelected()) {
                                        Node* node = single.state->nodeAt(event.mouseButton);
                                        single.state->toggleNode(node);
                                    }
                                    else {
                                        bool passed = single.state->addConnection(single.state->nodeAt(event.mouseButton));

                                        if (!passed) {
                                            single.state->removeConnection(single.state->nodeAt(event.mouseButton));

                                            if (!single.state->directed)
                                                single.state->removeConnection(single.state->getSelectedNode(), single.state->nodeAt(event.mouseButton));
                                        }

                                        single.state->deselectNode();
                                    }
                                }
                            }
                            else if (single.state->mode == GraphState::Mode::Edit) {
                                mouseHeld = true;
                                readyToSelect = true;

                                if (readyDouble)
                                    activateDouble = true;

                                readyDouble = true;
                            }
                        }
                        else if (event.mouseButton.button == sf::Mouse::Right) {
                            if (single.state->mode == GraphState::Mode::Edit)
                                single.state->deleteNode(single.state->nodeAt(event.mouseButton));
                            else if (single.state->mode == GraphState::Mode::Connect) {
                                if (single.state->isNodeSelected())
                                    single.state->deselectNode();
                            }
                        }
                    }
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (single.state->mode != GraphState::Mode::Typing) {
                    if (event.key.code == sf::Keyboard::C)
                        single.state->toggleConnectMode();
                    else if (event.key.code == sf::Keyboard::D)
                        single.state->toggleDirectedMode();
                    else if (event.key.code == sf::Keyboard::F)
                        single.state->toggleForce();
                    else if (event.key.code == sf::Keyboard::E)
                        single.state->mode = GraphState::Mode::Edit;
                }
            }
            else if (event.type == sf::Event::TextEntered) {
                if (single.state->mode == GraphState::Mode::Typing) {
                    int code = event.text.unicode;
                    Node *node = single.state->getSelectedNode();

                    std::string label = node->label;

                    if (code != 27) {
                        if (code == 8) {
                            if (!label.empty())
                                label = label.substr(0, label.size() - 1);
                        } else if (code != 13)
                            label += (char) code;

                        single.state->errorLabel = false;

                        if (code == 13) {
                            if (single.state->isLabelTaken(label, node) || label.empty()) {
                                single.state->errorLabel = true;
                            }
                            else
                                single.state->mode = single.DEFAULT_MODE;
                        }

                        node->label = label;
                    }
                    else {
                        node->label = prevLabel;

                        single.state->errorLabel = false;

                        single.state->mode = single.DEFAULT_MODE;
                    }
                }
            }
            else if (event.type == sf::Event::Resized) {
                single.state->getPositionGrid().updateResizeData(event.size);
            }
            else if (event.type == sf::Event::MouseMoved) {
                readyToSelect = false;
            }
        }
    }

    return 0;
}

void onMouseHeld(sf::Vector2f offset, Node* node) {
    Single& single = Single::instance();

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        auto pos = single.state->getPositionGrid().gl_loc(sf::Mouse::getPosition(single.window));
        sf::Vector2f ms(pos[0], pos[1]);
        single.state->changeNodePositionLocally(node, ms - offset);
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