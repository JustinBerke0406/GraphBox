#include <iostream>
#include "Single.h"

Single::Single() {
    state = new GraphState();

    window.create(sf::VideoMode(WIDTH, HEIGHT),
                  "Untitled - GraphBox");

    window.setFramerateLimit(60);

    if (!font.loadFromFile("resources/arial.ttf"))
        std::cout << "Unable to load arial.ttf";

    state->points=sf::VertexArray(sf::Points, WIDTH * HEIGHT);

    defaultView.reset(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    toolView.reset(sf::FloatRect(0, 0, WIDTH, TOOL_HEIGHT));

    float toolHeightPerc = (float)TOOL_HEIGHT / HEIGHT;

    defaultView.setViewport(sf::FloatRect(0.0f, toolHeightPerc, 1.0f, 1.0f));
    toolView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, toolHeightPerc));

    HWND hwnd;              // owner window

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Graph Files (*.graph)\0*.graph\0";
    ofn.lpstrDefExt = "graph";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = LPCSTR(R"(.\saves)");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

Single::~Single() {
    delete state;
}
