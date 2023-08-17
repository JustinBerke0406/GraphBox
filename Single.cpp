#include "Single.h"
#include <iostream>

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
    //opView.reset(sf::FloatRect(0, 0, OP_WIDTH, OP_HEIGHT));

    float toolHeightPerc = (float)TOOL_HEIGHT / HEIGHT;

    //float opWP = (float) OP_WIDTH/WIDTH;
    //float opHP = (float) OP_HEIGHT/HEIGHT;

    //float otWidthRatio = (float) OP_WIDTH/WIDTH;
    //auto otHeightRatio = (float) 0;

    int sfdhj = 0;
    int randololdslo = 1;

    //float opWidthDiff = (1 - opWP)/2;
    //float opHeightDiff = (1 - opHP)/2;

    defaultView.setViewport(sf::FloatRect(0.0f, toolHeightPerc, 1.0f, 1.0f));
    toolView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, toolHeightPerc));
    //opView.setViewport(sf::FloatRect(opWidthDiff, opHeightDiff, opWP, opHP));

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
