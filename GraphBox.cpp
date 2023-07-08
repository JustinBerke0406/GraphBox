#include <cmath>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <thread>
#include <tchar.h>
#include <filesystem>
#include "GraphBox.h"
#include "Single.h"

int launch() {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

    window.setKeyRepeatEnabled(false);

    // Event needed for window to work
    sf::Event event{};

    sf::Cursor cursor;

    InputHelper& inputs = single.inputHelper;

    bool defaultCursor = true;
    bool offsetSet = false;
    bool startDrag = false;

    std::string prevLabel;

    sf::Vector2f offset;
    sf::Vector2f dragPos;

    Node* draggedNode = nullptr;
    Node* lastClicked = nullptr;

    int timesNodeClicked = 0;

    // For loop to keep window open
    while (window.isOpen())
    {
        // Draw spriteHidden
        window.clear(sf::Color(255,255,255));

        window.setView(single.defaultView);

        inputs.registerEvents();

        // Render
        render();
        single.state->initToolbox();

        if (single.state->forceMode)
            single.state->physicsUpdate();

        if (inputs.didMultiPress(sf::Mouse::Left) && timesNodeClicked >= 2) {
            Node* node = single.state->nodeAt(sf::Mouse::getPosition(single.window));

            if (node != nullptr && node == lastClicked) {
                single.state->selectNode(node);
                single.state->mode = GraphState::Mode::Typing;

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
                if (single.window.getViewport(single.defaultView).contains(sf::Vector2<int>(event.mouseButton.x, event.mouseButton.y))) {
                    if (single.state->mode != GraphState::Mode::View) {
                        if (single.state->mode != GraphState::Mode::Typing) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                if (single.state->mode == GraphState::Mode::Edit) {
                                    if (single.state->cursorOverClickable()) {
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
                                else if (single.state->mode == GraphState::Mode::Connect) {
                                    if (single.state->wouldSelect(event.mouseButton)) {
                                        if (!single.state->isNodeSelected()) {
                                            Node *node = single.state->nodeAt(event.mouseButton);
                                            single.state->toggleNode(node);
                                        } else {
                                            bool passed = single.state->addConnection(
                                                    single.state->nodeAt(event.mouseButton));

                                            if (!passed) {
                                                single.state->removeConnection(single.state->nodeAt(event.mouseButton));

                                                if (!single.state->directed)
                                                    single.state->removeConnection(single.state->getSelectedNode(),
                                                                                   single.state->nodeAt(
                                                                                           event.mouseButton));
                                            }

                                            single.state->deselectNode();
                                        }
                                    }
                                }
                            } else if (event.mouseButton.button == sf::Mouse::Right) {
                                if (single.state->mode == GraphState::Mode::Edit) {
                                    Node* del = single.state->nodeAt(event.mouseButton);

                                    if (draggedNode == del) {
                                        draggedNode = nullptr;
                                    }

                                    single.state->deleteNode(del);
                                } else if (single.state->mode == GraphState::Mode::Connect) {
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
                }
                else if (single.window.getViewport(single.toolView).contains(sf::Vector2<int>(event.mouseButton.x, event.mouseButton.y))) {
                    std::string button = single.state->getButtonAtPoint(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    if (button == "Connect")
                        single.state->toggleConnectMode();
                    else if (button == "Directed")
                        single.state->toggleDirectedMode();
                    else if (button == "Force")
                        single.state->toggleForce();
                    else if (button == "Adjacency")
                        single.state->toggleAdjMode();
                    else if (button == "Print") {
                        if (single.fileName.empty())
                            saveFile();

                        if (!single.fileName.empty())
                            print();
                    }
                    else if (button == "New")
                        single.state->reset();
                    else if (button == "Save As")
                        saveFile();
                    else if (button == "Load")
                        loadFile();
                    else if (button == "Save") {
                        if (single.fileName.empty())
                            saveFile();
                        else
                            single.state->saveFile(single.fileName);
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
                    else if (event.key.code == sf::Keyboard::I) {
                        if (single.state->forceMode) {
                            single.state->invertForce();
                        }
                    }
                    else if (event.key.code == sf::Keyboard::E)
                        single.state->mode = GraphState::Mode::Edit;
                    else if (event.key.code == sf::Keyboard::A)
                        single.state->toggleAdjMode();
                    else if (event.key.code == sf::Keyboard::P)
                        print();
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
            else if (event.type == sf::Event::MouseMoved) {

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

void render() {
    Single& single = Single::instance();

    sf::RenderWindow& window = single.window;

    if (single.state->densityMode)
        single.state->densityThreadCaller();

    single.state->drawNodes();
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

        if (single.state->directed) {
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

        if (single.state->directed) {
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

    auto fileNames = single.OUTPUT_FILE + fileName(single.fileName) + ((single.state->directed) ? "_Directed" : "") + ".txt";
    std::ofstream out(fileNames);

    if (!out.is_open())
        std::cout << "Unable to create print file";

    out << list;
    out.close();
}

void registerMovement() {
    Single& single = Single::instance();

    InputHelper& inputs = single.inputHelper;

    if (inputs.isPressed(sf::Keyboard::Equal)) {
        single.defaultView.zoom(1.0f/single.ZOOM_SPEED);
    }

    if (inputs.isPressed(sf::Keyboard::Hyphen)) {
        single.defaultView.zoom(single.ZOOM_SPEED);
    }

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

void loadFile() {
    Single& single = Single::instance();

    HANDLE hf;              // file handle

    auto& ofn = single.ofn;
    ofn.lpstrFile[0] = '\0';

    if (GetOpenFileName(&ofn) == TRUE) {
        hf = CreateFile(ofn.lpstrFile,
                        GENERIC_READ,
                        0,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

        CloseHandle(hf);

        single.state->loadFile(single.szFile);

        single.window.setTitle(fileName(single.szFile) + " - GraphBox");

        single.fileName = single.szFile;

        std::filesystem::current_path("../");
    }
}

void saveFile() {
    Single& single = Single::instance();

    HANDLE hf;              // file handle

    auto& ofn = single.ofn;

    ofn.lpstrFile = single.szFile;

    if (GetSaveFileName(&ofn) == TRUE) {
        hf = CreateFile(ofn.lpstrFile,
                        GENERIC_READ,
                        0,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

        CloseHandle(hf);

        single.state->saveFile(single.szFile);

        single.window.setTitle(fileName(single.szFile) + " - GraphBox");

        single.fileName = single.szFile;

        std::filesystem::current_path("../");
    }
}

std::string fileName(std::string path) {
    std::string wExt = path.substr(path.find_last_of('\\')+1);

    return wExt.substr(0, wExt.size()-6);
}

int main() {
    return launch();
}