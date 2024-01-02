#include "Single.h"
#include <iostream>
#include <filesystem>

Single::Single() {
    CHAR pBuf[MAX_PATH];
    GetModuleFileNameA(nullptr, pBuf, MAX_PATH);

    std::filesystem::current_path(std::filesystem::path(pBuf).parent_path());

    HWND hwnd = GetDesktopWindow();              // owner window

    RECT desktopRect;
    GetWindowRect(hwnd, &desktopRect);

    WIDTH = desktopRect.right;
    HEIGHT = desktopRect.bottom;

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

    state = new GraphState();

    window.create(sf::VideoMode(WIDTH/2, HEIGHT/2),
                  "Untitled - GraphBox");

    ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);

    window.setFramerateLimit(60);

    if (!font.loadFromFile("resources/arial.ttf"))
        std::cout << "Unable to load arial.ttf";

    state->points=sf::VertexArray(sf::Points, WIDTH * HEIGHT);

    defaultView.reset(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    toolView.reset(sf::FloatRect(0, 0, WIDTH, TOOL_HEIGHT));
    opView.reset(sf::FloatRect(0, 0, OP_WIDTH_PER*WIDTH, OP_HEIGHT_PER*HEIGHT));

    float toolHeightPerc = (float)TOOL_HEIGHT / HEIGHT;

    float opWidthDiff = (1 - OP_WIDTH_PER)/2;
    float opHeightDiff = (1 - OP_HEIGHT_PER)/2;

    defaultView.setViewport(sf::FloatRect(0.0f, toolHeightPerc, 1.0f, 1.0f));
    toolView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, toolHeightPerc));
    opView.setViewport(sf::FloatRect(opWidthDiff, opHeightDiff, OP_WIDTH_PER, OP_HEIGHT_PER));
}

Single::~Single() {
    delete state;
}
