#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

using namespace sf;

class InputHelper {
private:
    Clock clock;
    Time currentTime;

    std::vector<std::pair<Time, Time>> keyTime;
    std::vector<std::pair<Time, Time>> buttonTime;

    std::vector<int> simKey;
    std::vector<int> simButton;

    std::unordered_map<Keyboard::Key, std::function<void()>> keyReleaseQueue;
    std::unordered_map<Mouse::Button, std::function<void()>> buttonReleaseQueue;

    std::unordered_map<Keyboard::Key, std::function<void()>> keyHeldLongQueue;
    std::unordered_map<Mouse::Button, std::function<void()>> buttonHeldLongQueue;

    struct MousePositionInfo {
        sf::Vector2i lastPosition;
        sf::Vector2i currentPosition;

        Time lastMoved;
    };

    MousePositionInfo mousePositionInfo;

    void reset();

    void runApplicableReleaseQueues();
    void runApplicableHeldLongQueues();
public:
    InputHelper();

    void registerEvents();

    bool isPressed(Keyboard::Key key) const;
    bool isPressed(Mouse::Button button) const;

    Time timeSinceRelease(Keyboard::Key key) const;
    Time timeSinceRelease(Mouse::Button button) const;

    Time timeCurrentlyHeld(Keyboard::Key key) const;
    Time timeCurrentlyHeld(Mouse::Button button) const;

    Time timeHeld(Keyboard::Key key) const;
    Time timeHeld(Mouse::Button button) const;

    Time pressedAt(Keyboard::Key key) const;
    Time pressedAt(Mouse::Button button) const;
    
    int getConcurrentActionsAmount(Keyboard::Key key) const;
    int getConcurrentActionsAmount(Mouse::Button button) const;

    bool didDoublePress(Keyboard::Key key) const;
    bool didDoublePress(Mouse::Button button) const;

    bool didMultiPress(Keyboard::Key key) const;
    bool didMultiPress(Mouse::Button button) const;

    bool isHeldLong(Keyboard::Key key) const;
    bool isHeldLong(Mouse::Button button) const;

    bool wasHeldLong(Keyboard::Key key) const;
    bool wasHeldLong(Mouse::Button button) const;

    bool justReleased(Keyboard::Key key) const;
    bool justReleased(Mouse::Button button) const;

    void queueReleaseEvent(Keyboard::Key key, std::function<void()> func);
    void queueReleaseEvent(Mouse::Button, std::function<void()> func);

    void queueHeldLongEvent(Keyboard::Key key, std::function<void()> func);
    void queueHeldLongEvent(Mouse::Button, std::function<void()> func);

    Time mouseLastMoved() const;
    bool hasMouseMovedSince(Time start) const;
};
