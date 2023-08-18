#include "ViewRenderer.h"

void ViewRenderer::renderToolbar() {
    Single& single = Single::instance();

    single.window.setView(single.toolView);

    auto toolViewSize = single.toolView.getSize();

    int toolButtons = single.TOOL_BUTTONS.size();

    float width = toolViewSize.x / toolButtons;

    for (int i = 0; i < toolButtons; i++) {
        sf::RectangleShape button(sf::Vector2f(width, toolViewSize.y));
        button.setPosition(width * i, 0);
        button.setFillColor(sf::Color(220, 220, 220));
        button.setOutlineThickness(-2);
        button.setOutlineColor(sf::Color::Black);

        auto mousePos = single.window.mapPixelToCoords(sf::Mouse::getPosition(single.window));

        std::string name = single.TOOL_BUTTONS[i];

        GraphState& state = *single.state;

        if ((single.mode["force"] && name == "Force") || (single.mode["adj"] && name == "Adjacency") ||
            (single.mode["directed"] && name == "Directed") || (single.mode.mainMode == ModeHandler::Mode::Connect && name == "Connect") ||
            (single.mode["opt"] && name == "Options"))
            button.setFillColor(single.HIGHLIGHT_COLOR);

        if (mousePos.y <= toolViewSize.y && mousePos.y >= 0 && mousePos.x > width*i && mousePos.x <= width*(i+1)) {
            auto curColor = button.getFillColor();

            button.setFillColor(sf::Color(curColor.r - 30, curColor.g - 30, curColor.b - 30));
        }

        sf::Text text;
        text.setFont(single.font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);
        text.setString(name);
        text.setOrigin(text.getLocalBounds().width/2, text.getLocalBounds().height/2);
        text.setPosition(width * (i+0.5), toolViewSize.y/2-5);

        single.window.draw(button);
        single.window.draw(text);
    }

    single.window.setView(single.defaultView);
}

void ViewRenderer::renderOptions() {
    Single& single = Single::instance();

    single.window.setView(single.opView);

    const int& WIDTH = single.OP_WIDTH, HEIGHT = single.OP_HEIGHT;

    auto rect = RectangleShape(Vector2f(WIDTH, HEIGHT));
    rect.setFillColor(Color(230,230,230));
    rect.setOutlineThickness(-2);
    rect.setOutlineColor(Color(130,130,130));

    single.window.draw(rect);

    sf::Text text;
    text.setFont(single.font);
    text.setCharacterSize(30);
    text.setFillColor(Color::Black);
    text.setString("Options");
    text.setOrigin(text.getLocalBounds().width/2, 0);
    text.setPosition(WIDTH/2, 5);

    single.window.draw(text);

    text.setFont(single.font);
    text.setCharacterSize(24);
    text.setFillColor(Color::Black);
    text.setString("Force: Base Friction");
    text.setOrigin(0, text.getLocalBounds().height/2);
    text.setPosition(20, 105);

    single.window.draw(text);

    Slider frSl = Slider(350, 100, 0, 250, single.physicsEngine.frictionMult * 100);
    frSl.draw(single.window);

    single.physicsEngine.frictionMult = frSl.getValue()/100;

    text.setFont(single.font);
    text.setCharacterSize(24);
    text.setFillColor(Color::Black);
    text.setString("Force: Edge Friction");
    text.setOrigin(0, text.getLocalBounds().height/2);
    text.setPosition(20, 155);

    single.window.draw(text);

    Slider spFrSl = Slider(350, 150, 0, 250, single.physicsEngine.spFrictionMult * 100);
    spFrSl.draw(single.window);

    single.physicsEngine.spFrictionMult = spFrSl.getValue()/100;

    text.setFont(single.font);
    text.setCharacterSize(24);
    text.setFillColor(Color::Black);
    text.setString("Force: Edge Rest Length");
    text.setOrigin(0, text.getLocalBounds().height/2);
    text.setPosition(20, 205);

    single.window.draw(text);

    Slider restLenSl = Slider(350, 200, 0, 10, single.physicsEngine.springRestLen);
    restLenSl.draw(single.window);

    single.physicsEngine.springRestLen = restLenSl.getValue();

    single.window.setView(single.defaultView);
}