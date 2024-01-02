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

    const int& WIDTH = single.OP_WIDTH_PER*single.WIDTH, HEIGHT = single.OP_HEIGHT_PER*single.HEIGHT;

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

    auto write = [&text, &single](const std::string& msg, int y) {
        text.setFont(single.font);
        text.setCharacterSize(24);
        text.setFillColor(Color::Black);
        text.setString(msg);
        text.setOrigin(0, text.getLocalBounds().height/2);
        text.setPosition(20, y);

        single.window.draw(text);
    };

    write("Force: Base Friction", 105);

    Slider& bSlider = single.sliderSettings.at("baseFriction");
    bSlider.draw(single.window);

    single.physicsEngine.frictionMult = bSlider.getValue()/100;

    write("Force: Edge Friction", 155);

    Slider& eSlider = single.sliderSettings.at("edgeFriction");
    eSlider.draw(single.window);

    single.physicsEngine.spFrictionMult = eSlider.getValue()/100;

    write("Force: Edge Rest Length", 205);

    Slider& rSlider = single.sliderSettings.at("restLength");
    rSlider.draw(single.window);

    single.physicsEngine.springRestLen = rSlider.getValue();

    write("Force: Invert", 255);

    Checkbox& iCheckbox = single.checkSettings.at("forceInverse");

    iCheckbox.draw(single.window);

    single.mode["inv"] = iCheckbox.getValue();

    write("1-Indexing", 355);

    Checkbox& oCheckbox = single.checkSettings.at("oneIndexing");

    oCheckbox.draw(single.window);

    single.oneIndexing = oCheckbox.getValue();

    single.window.setView(single.defaultView);
}