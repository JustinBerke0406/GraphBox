#include <cmath>
#include <iostream>
#include <unordered_set>
#include <thread>
#include <tchar.h>
#include "GraphBox.h"
#include "Single.h"
#include "FileManager.h"
#include "helper/ViewHelper.h"
#include "rendering/Renderer.h"
#include "PhysicsEngine.h"

int launch(std::string filepath) {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

    window.setKeyRepeatEnabled(false);

    // Event needed for window to work
    sf::Event event{};

    sf::Cursor cursor;

    InputHelper& inputs = InputHelper::instance();

    bool defaultCursor = true;
    bool offsetSet = false;
    bool startDrag = false;

    std::string prevLabel;

    sf::Vector2f offset;
    sf::Vector2f dragPos;

    Node* draggedNode = nullptr;
    Node* lastClicked = nullptr;

    GraphData* graphData = single.graphData;

    int timesNodeClicked = 0;

    Renderer render;

    if (!filepath.empty())
        FileManager::load(filepath);

    // For loop to keep window open
    while (window.isOpen())
    {
        // Draw spriteHidden
        window.clear(sf::Color(255,255,255));

        window.setView(single.defaultView);

        inputs.registerEvents();

        graphData->updateDegrees();

        // Render
        render();

        if (single.mode["force"])
            single.physicsEngine.update();

        if (inputs.didMultiPress(sf::Mouse::Left) && timesNodeClicked >= 2) {
            Node* node = single.state->nodeAt(sf::Mouse::getPosition(single.window));

            if (node != nullptr && node == lastClicked) {
                single.state->selectNode(node);
                single.mode["type"] = true;

                prevLabel = node->label;

                offsetSet = false;
            }
        }

        // Screen dragging
        if (startDrag) {
            if (inputs.isPressed(sf::Mouse::Middle) || inputs.isPressed(sf::Mouse::Left)) {
                auto tempPos = sf::Mouse::getPosition(single.window);
                auto nPos = single.window.mapPixelToCoords(tempPos);

                single.defaultView.move(dragPos - nPos);
                single.window.setView(single.defaultView);

                nPos = single.window.mapPixelToCoords(tempPos);

                dragPos = nPos;
            }
            else {
                startDrag = false;
            }
        }

        if (inputs.isPressed(sf::Mouse::Left) && draggedNode != nullptr) {
            auto mousePos = sf::Mouse::getPosition(single.window);

            if (!offsetSet) {
                auto loMouse = single.window.mapPixelToCoords(mousePos);

                offset = sf::Vector2f(loMouse.x - draggedNode->x, loMouse.y - draggedNode->y);

                offsetSet = true;
            }

            onMouseHeld(offset, draggedNode);

            draggedNode->velocity = sf::Vector2f(0, 0);
        }
        else {
            offsetSet = false;
            draggedNode = nullptr;
        }

        // draw method
        window.display();

        // Movement and zooming
        registerMovement();

        if (ViewHelper::cursorOverClickable()) {
            cursor.loadFromSystem(sf::Cursor::Hand);
            window.setMouseCursor(cursor);

            defaultCursor = false;
        }
        else if (!ViewHelper::cursorOverClickable() && !defaultCursor) {
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
                if (single.window.getViewport(single.defaultView).contains(sf::Vector2<int>(event.mouseButton.x, event.mouseButton.y)) &&
                !(single.window.getViewport(single.opView).contains(sf::Vector2<int>(event.mouseButton.x, event.mouseButton.y)) && single.mode["opt"])) {
                    if (!single.mode["type"]) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (single.mode.mainMode == ModeHandler::Mode::Edit) {
                                if (ViewHelper::cursorOverClickable()) {
                                    auto button = event.mouseButton;

                                    draggedNode = single.state->nodeAt(sf::Mouse::getPosition(single.window));

                                    inputs.queueReleaseEvent(sf::Mouse::Left, [button, &single, &inputs, &lastClicked, &timesNodeClicked]() {
                                        if (!inputs.hasMouseMovedSince(inputs.pressedAt(sf::Mouse::Left))) {
                                            Node* pastNode = single.state->nodeAt(button);
                                            Node* node = single.state->nodeAt(sf::Mouse::getPosition(single.window));

                                            if (pastNode == node) {
                                                single.state->toggleNode(node);

                                                if (lastClicked != node)
                                                    timesNodeClicked = 0;

                                                lastClicked = node;
                                                timesNodeClicked++;
                                            }
                                        }
                                    });
                                }
                                else {
                                    inputs.queueReleaseEvent(sf::Mouse::Left, [&single, &inputs, &lastClicked, &timesNodeClicked]() {
                                        auto globalMousePos = sf::Mouse::getPosition(single.window);

                                        if (!inputs.hasMouseMovedSince(inputs.pressedAt(sf::Mouse::Left)) || !inputs.wasHeldLong(sf::Mouse::Left)) {
                                            single.state->createNode(single.window.mapPixelToCoords(globalMousePos));
                                            lastClicked = nullptr;
                                            timesNodeClicked = 0;
                                        }
                                    });

                                    inputs.queueHeldLongEvent(sf::Mouse::Left, [&single, &startDrag, &dragPos]() {
                                        auto globalMousePos = sf::Mouse::getPosition(single.window);

                                        if (!startDrag) {
                                            startDrag = true;

                                            dragPos = single.window.mapPixelToCoords(
                                                    sf::Vector2i(globalMousePos.x, globalMousePos.y));
                                        }
                                    });
                                }
                            }
                            else if (single.mode.mainMode == ModeHandler::Mode::Connect) {
                                if (single.state->wouldSelect(event.mouseButton)) {
                                    if (!single.state->isNodeSelected()) {
                                        Node *node = single.state->nodeAt(event.mouseButton);
                                        single.state->toggleNode(node);
                                    } else {
                                        bool passed = single.state->addConnection(
                                                single.state->nodeAt(event.mouseButton));

                                        if (!passed) {
                                            single.state->removeConnection(single.state->nodeAt(event.mouseButton));

                                            if (!single.mode["directed"])
                                                single.state->removeConnection(single.state->getSelectedNode(),
                                                                               single.state->nodeAt(
                                                                                       event.mouseButton));
                                        }

                                        single.state->deselectNode();
                                    }
                                }
                            }
                        } else if (event.mouseButton.button == sf::Mouse::Right) {
                            if (single.mode.mainMode == ModeHandler::Mode::Edit) {
                                Node* del = single.state->nodeAt(event.mouseButton);

                                if (draggedNode == del)
                                    draggedNode = nullptr;

                                single.state->deleteNode(del);
                            } else if (single.mode.mainMode == ModeHandler::Mode::Connect) {
                                if (single.state->isNodeSelected())
                                    single.state->deselectNode();
                            }
                        } else if (event.mouseButton.button == sf::Mouse::Middle) {
                            if (!startDrag) {
                                startDrag = true;

                                dragPos = single.window.mapPixelToCoords(
                                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                            }
                        }
                    }
                }
                else if (single.window.getViewport(single.toolView).contains(sf::Vector2<int>(event.mouseButton.x, event.mouseButton.y))) {
                    std::string button = ViewHelper::getToolboxButtonAtPoint(
                            sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    if (button == "Connect")
                        single.mode.mainToggle();
                    else if (button == "Directed")
                        single.mode.toggle("directed");
                    else if (button == "Force") {
                        single.mode.toggle("force");

                        if (!single.mode["force"])
                            PhysicsEngine::freezeAll();
                    }
                    else if (button == "Adjacency")
                        single.mode.toggle("adj");
                    else if (button == "Print") {
                        if (single.fileName.empty())
                            FileManager::saveDialog();

                        if (!single.fileName.empty())
                            print();
                    }
                    else if (button == "New")
                        single.state->reset();
                    else if (button == "Save As")
                        FileManager::saveDialog();
                    else if (button == "Load")
                        FileManager::loadDialog();
                    else if (button == "Save") {
                        if (single.fileName.empty())
                            FileManager::saveDialog();
                        else
                            FileManager::save(single.fileName);
                    }
                    else if (button == "Options")
                        single.mode.toggle("opt");
                    else if (button == "View")
                        single.mode.toggle("mask");
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (!single.mode["type"] && !inputs.isCtrlPressed()) {
                    if (event.key.code == sf::Keyboard::C)
                        single.mode.mainToggle();
                    else if (event.key.code == sf::Keyboard::D)
                        single.mode.toggle("directed");
                    else if (event.key.code == sf::Keyboard::F) {
                        single.mode.toggle("force");

                        if (!single.mode["force"])
                            PhysicsEngine::freezeAll();
                    }
                    else if (event.key.code == sf::Keyboard::I) {
                        if (single.mode["force"]) {
                            single.mode.toggle("inv");
                        }
                    }
                    else if (event.key.code == sf::Keyboard::E)
                        single.mode.mainMode = ModeHandler::Mode::Edit;
                    else if (event.key.code == sf::Keyboard::A)
                        single.mode.toggle("adj");
                    else if (event.key.code == sf::Keyboard::P)
                        print();
                    else if (event.key.code == sf::Keyboard::Space) {
                        Node* node = single.state->getSelectedNode();

                        if (node != nullptr)
                            node->locked = !node->locked;
                    }
                    else if (event.key.code == sf::Keyboard::O) {
                        single.mode.toggle("opt");
                    }
                    else if (event.key.code == sf::Keyboard::Escape && single.mode["opt"]) {
                        single.mode["opt"] = false;
                    }
                    else if (event.key.code == sf::Keyboard::V) {
                        single.mode.toggle("mask");
                    }
                    else if (event.key.code == sf::Keyboard::Escape && single.mode["mask"]) {
                        single.mode["mask"] = false;
                    }
                }
            }
            else if (event.type == sf::Event::TextEntered) {
                if (single.mode["type"] && !inputs.isCtrlPressed()) {
                    int code = event.text.unicode;
                    Node *node = single.state->getSelectedNode();

                    std::string label = node->label;

                    if (code != 27) {
                        if (code == 8) {
                            if (!label.empty())
                                label = label.substr(0, label.size() - 1);
                        } else if (code != 13)
                            label += (char) code;

                        single.mode["error"] = false;

                        if (code == 13) {
                            if (single.state->isLabelTaken(label, node) || label.empty()) {
                                single.mode["error"] = true;
                            }
                            else
                                single.mode["type"] = false;
                        }

                        node->label = label;
                    }
                    else {
                        node->label = prevLabel;

                        single.mode["error"] = false;

                        single.mode["type"] = false;
                    }
                }
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                sf::Vector2i mousePos(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                auto conv = sf::Vector2i(mousePos.x, mousePos.y);

                auto prevCoord = single.window.mapPixelToCoords(conv);
                single.defaultView.zoom(1.0f/std::pow(single.ZOOM_SPEED*single.WHEEL_SENS, event.mouseWheelScroll.delta));
                single.window.setView(single.defaultView);

                auto postCoord = single.window.mapPixelToCoords(conv);

                single.defaultView.move(prevCoord - postCoord);
            }
            else if(event.type == sf::Event::GainedFocus)
                single.focused = true;
            else if(event.type == sf::Event::LostFocus)
                single.focused = false;
        }
    }

    return 0;
}

void onMouseHeld(sf::Vector2f offset, Node* node) {
    Single& single = Single::instance();

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        auto pos = single.window.mapPixelToCoords(sf::Mouse::getPosition(single.window));
        single.state->changeNodePositionLocally(node, pos - offset);
    }
}

void print() {
    Single& single = Single::instance();

    std::string list;

    auto nodes = single.state->getNodes();

    if (!single.indexOrdering) {
        auto doQuote = [](const std::string &str) {
            for (auto c: str) {
                if (isspace(c))
                    return "\"" + str + "\"";
            }

            return str;
        };

        if (single.mode["directed"]) {
            std::vector<std::pair<std::string, std::string>> combo;

            bool dup;

            for (auto &node: nodes) {
                for (Node *n: node->connections)
                    combo.emplace_back(node->label, n->label);
            }

            for (Node *n: nodes) {
                if (n->connections.empty()) {
                    dup = false;

                    for (const std::pair<std::string, std::string> &p: combo)
                        if (p.first == n->label || p.second == n->label) dup = true;

                    if (!dup)
                        list += doQuote(n->label) + "\n";
                }
            }

            for (const auto &p: combo)
                list += doQuote(p.first) + " " + doQuote(p.second) + "\n";
        } else {
            auto cmp = [](std::pair<std::string, std::string> a, std::pair<std::string, std::string> b) {
                return (a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first);
            };

            std::vector<std::pair<std::string, std::string>> combo;

            bool dup;

            for (auto &node: nodes) {
                for (Node *n: node->connections) {
                    std::pair<std::string, std::string> a1(node->label, n->label);

                    dup = false;

                    for (const std::pair<std::string, std::string> &p: combo)
                        if (cmp(p, a1)) dup = true;

                    if (!dup)
                        combo.push_back(a1);
                }
            }

            for (Node *n: nodes) {
                if (n->connections.empty()) {
                    dup = false;

                    for (const std::pair<std::string, std::string> &p: combo)
                        if (p.first == n->label || p.second == n->label) dup = true;

                    if (!dup)
                        list += doQuote(n->label) + "\n";
                }
            }

            for (const auto &p: combo)
                list += doQuote(p.first) + " " + doQuote(p.second) + "\n";
        }
    }
    else {
        auto cmp = [](std::pair<Node*, Node*> a, std::pair<Node*, Node*> b) {
            return (a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first);
        };

        std::vector<std::pair<Node*, Node*>> combo;

        if (single.mode["directed"]) {
            bool dup;

            for (auto &node: nodes) {
                for (Node *n: node->connections)
                    combo.emplace_back(node, n);
            }

            for (Node *n: nodes) {
                if (n->connections.empty()) {
                    dup = false;

                    for (auto &p: combo)
                        if (p.first == n || p.second == n) dup = true;

                    if (!dup)
                        list += std::to_string(single.state->getNodeIndex(n)) + "\n";
                }
            }

            for (const auto &p: combo)
                list += "(" + std::to_string(single.state->getNodeIndex(p.first)) + "," +
                        std::to_string(single.state->getNodeIndex(p.second)) + ")\n";
        }
        else {
            bool dup;

            for (auto &node: nodes) {
                for (Node *n: node->connections) {
                    std::pair<Node*, Node*> a1(node, n);

                    dup = false;

                    for (const auto &p: combo)
                        if (cmp(p, a1)) dup = true;

                    if (!dup)
                        combo.push_back(a1);
                }
            }

            for (Node *n: nodes) {
                if (n->connections.empty()) {
                    dup = false;

                    for (const auto &p: combo)
                        if (p.first == n || p.second == n) dup = true;

                    if (!dup)
                        list += std::to_string(single.state->getNodeIndex(n)) + "\n";
                }
            }

            for (const auto &p: combo)
                list += "(" + std::to_string(single.state->getNodeIndex(p.first)) + "," +
                        std::to_string(single.state->getNodeIndex(p.second)) + ")\n";
        }
    }

    std::string wExt = single.fileName.substr(single.fileName.find_last_of('\\')+1);

    auto fileNames = single.OUTPUT_FILE + wExt.substr(0, wExt.size()-6) + ((single.mode["directed"]) ? "_Directed" : "") + ".txt";
    std::ofstream out(fileNames);

    if (!out.is_open())
        std::cout << "Unable to create print file";

    out << list;
    out.close();
}

void registerMovement() {
    Single& single = Single::instance();

    InputHelper& inputs = InputHelper::instance();

    if (!single.mode["type"]) {
        if (inputs.isPressed(sf::Keyboard::Left)) {
            single.defaultView.move(-single.MOVE_SPEED, 0);
        }

        if (inputs.isPressed(sf::Keyboard::Right)) {
            single.defaultView.move(single.MOVE_SPEED, 0);
        }

        if (inputs.isPressed(sf::Keyboard::Up)) {
            single.defaultView.move(0, -single.MOVE_SPEED);
        }

        if (inputs.isPressed(sf::Keyboard::Down)) {
            single.defaultView.move(0, single.MOVE_SPEED);
        }
    }

    if (inputs.isCtrlPressed() && single.focused) {
        if (inputs.isPressed(sf::Keyboard::C)) {
            if (single.state->isNodeSelected()) {
                sf::Clipboard::setString(single.state->getSelectedNode()->label);
            }
        }

        if (inputs.isPressed(sf::Keyboard::V)) {
            if (single.state->isNodeSelected()) {
                single.state->getSelectedNode()->label = sf::Clipboard::getString();
            }
        }

        if (inputs.isPressed(sf::Keyboard::S)) {
            if (single.fileName.empty())
                FileManager::saveDialog();
            else
                FileManager::save(single.fileName);
        }
    }
    else if (!single.mode["type"]) {
        if (inputs.isPressed(sf::Keyboard::Equal)) {
            single.defaultView.zoom(1.0f/single.ZOOM_SPEED);
        }

        if (inputs.isPressed(sf::Keyboard::Hyphen)) {
            single.defaultView.zoom(single.ZOOM_SPEED);
        }
    }
}

int CALLBACK WinMain(
        HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        int         nCmdShow
) {
    std::string filepath(lpCmdLine);

    if (!filepath.empty())
        filepath = filepath.substr(1, filepath.length()-2);

    return launch(filepath);
}