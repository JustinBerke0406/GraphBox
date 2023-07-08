#include "InputHelper.h"
#include "Single.h"

InputHelper::InputHelper() {
    reset();
}

void InputHelper::reset() {
    Time start = milliseconds(-(Single::DOUBLE_CLICK_TIME + 2));
    Time end = milliseconds(-(Single::DOUBLE_CLICK_TIME + 1));

    currentTime = Time::Zero;

    keyTime.clear();
    buttonTime.clear();

    simKey.clear();
    simButton.clear();

    keyReleaseQueue.clear();
    buttonReleaseQueue.clear();

    for (int i = 0; i < 102; i++) {
        keyTime.emplace_back(start, end);
        simKey.push_back(0);
    }

    for (int i = 0; i < 5; i++) {
        buttonTime.emplace_back(start, end);
        simButton.push_back(0);
    }

    clock.restart();

    mousePositionInfo.lastPosition = sf::Mouse::getPosition();
    mousePositionInfo.currentPosition = sf::Mouse::getPosition();
    mousePositionInfo.lastMoved = start;
}

void InputHelper::runApplicableReleaseQueues() {
    std::vector<Keyboard::Key> usedKeys;
    std::vector<Mouse::Button> usedButtons;

    for (auto& item : keyReleaseQueue) {
        if (justReleased(item.first)) {
            item.second();

            usedKeys.push_back(item.first);
        }
    }

    for (auto& item : buttonReleaseQueue) {
        if (justReleased(item.first)) {
            item.second();

            usedButtons.push_back(item.first);
        }
    }

    for (auto& item : usedKeys)
        keyReleaseQueue.erase(item);

    for (auto& item : usedButtons)
        buttonReleaseQueue.erase(item);
}

void InputHelper::runApplicableHeldLongQueues() {
    std::vector<Keyboard::Key> usedKeys;
    std::vector<Mouse::Button> usedButtons;

    for (auto& item : keyHeldLongQueue) {
        if (wasHeldLong(item.first)) {
            item.second();

            usedKeys.push_back(item.first);
        }

        if (!isPressed(item.first))
            usedKeys.push_back(item.first);
    }

    for (auto& item : buttonHeldLongQueue) {
        if (wasHeldLong(item.first)) {
            item.second();

            usedButtons.push_back(item.first);
        }

        if (!isPressed(item.first))
            usedButtons.push_back(item.first);
    }

    for (auto& item : usedKeys)
        keyHeldLongQueue.erase(item);

    for (auto& item : usedButtons)
        buttonHeldLongQueue.erase(item);
}

void InputHelper::registerEvents() {
    currentTime = clock.getElapsedTime();

    // Update all keys
    for (int i = 0; i < 102; i++) {
        auto key = (Keyboard::Key) i;
        bool keyAlreadyPressed = isPressed(key);

        if (Keyboard::isKeyPressed(key)) {
            if (!keyAlreadyPressed) {
                keyTime[i].first = currentTime;

                if (timeSinceRelease(key).asMilliseconds() <= Single::DOUBLE_CLICK_TIME)
                    simKey[i]++;
                else
                    simKey[i] = 0;
            }
        }
        else {
            if (keyAlreadyPressed) {
                keyTime[i].second = currentTime;

                if (simKey[i] > 0 && timeCurrentlyHeld(key).asMilliseconds() <= Single::HOLD_TIME)
                    simKey[i]++;
                else
                    simKey[i] = 0;
            } else if (simKey[i] > 0 && timeSinceRelease(key).asMilliseconds() > Single::DOUBLE_CLICK_TIME)
                simKey[i] = 0;
        }
    }

    // Update all buttons
    for (int i = 0; i < 5; i++) {
        auto button = (Mouse::Button) i;
        bool buttonAlreadyPressed = isPressed(button);

        if (Mouse::isButtonPressed(button)) {
            if (!buttonAlreadyPressed) {
                buttonTime[i].first = currentTime;

                if (timeSinceRelease(button).asMilliseconds() <= Single::DOUBLE_CLICK_TIME)
                    simButton[i]++;
                else
                    simButton[i] = 0;
            }
        }
        else {
            if (buttonAlreadyPressed) {
                buttonTime[i].second = currentTime;

                if (simButton[i] > 0 && timeCurrentlyHeld(button).asMilliseconds() <= Single::HOLD_TIME)
                    simButton[i]++;
                else
                    simButton[i] = 0;
            } else if (simButton[i] > 0 && timeSinceRelease(button).asMilliseconds() > Single::DOUBLE_CLICK_TIME)
                simButton[i] = 0;
        }
    }

    auto mousePos = sf::Mouse::getPosition();

    if (mousePositionInfo.currentPosition != mousePos) {
        mousePositionInfo.lastPosition = mousePositionInfo.currentPosition;
        mousePositionInfo.currentPosition = mousePos;
        mousePositionInfo.lastMoved = currentTime;
    }

    runApplicableHeldLongQueues();
    runApplicableReleaseQueues();
}

bool InputHelper::isPressed(Keyboard::Key key) const {
    auto times = keyTime[key];

    return times.second < times.first;
}

bool InputHelper::isPressed(Mouse::Button button) const {
    auto times = buttonTime[button];

    return times.second < times.first;
}

Time InputHelper::timeSinceRelease(Keyboard::Key key) const {
    return clock.getElapsedTime() - keyTime[key].second;
}

Time InputHelper::timeSinceRelease(Mouse::Button button) const {
    return clock.getElapsedTime() - buttonTime[button].second;
}

Time InputHelper::timeCurrentlyHeld(Keyboard::Key key) const {
    auto times = buttonTime[key];

    if (isPressed(key))
        return clock.getElapsedTime() - times.first;
    else
        return Time::Zero;
}

Time InputHelper::timeCurrentlyHeld(Mouse::Button button) const {
    auto times = buttonTime[button];

    if (isPressed(button))
        return clock.getElapsedTime() - times.first;
    else
        return Time::Zero;
}

Time InputHelper::timeHeld(Keyboard::Key key) const {
    auto times = buttonTime[key];

    if (isPressed(key))
        return clock.getElapsedTime() - times.first;
    else
        return times.second - times.first;
}

Time InputHelper::timeHeld(Mouse::Button button) const {
    auto times = buttonTime[button];

    if (isPressed(button))
        return clock.getElapsedTime() - times.first;
    else
        return times.second - times.first;
}

Time InputHelper::pressedAt(Keyboard::Key key) const {
    return keyTime[key].first;
}

Time InputHelper::pressedAt(Mouse::Button button) const {
    return buttonTime[button].first;
}

int InputHelper::getConcurrentActionsAmount(Keyboard::Key key) const {
    return (simKey[key] / 2) + 1;
}

int InputHelper::getConcurrentActionsAmount(Mouse::Button button) const {
    return (simButton[button] / 2) + 1;
}

bool InputHelper::didDoublePress(Keyboard::Key key) const {
    return getConcurrentActionsAmount(key) == 2;
}

bool InputHelper::didDoublePress(Mouse::Button button) const {
    return getConcurrentActionsAmount(button) == 2;
}

bool InputHelper::didMultiPress(Keyboard::Key key) const {
    return getConcurrentActionsAmount(key) >= 2;
}

bool InputHelper::didMultiPress(Mouse::Button button) const {
    return getConcurrentActionsAmount(button) >= 2;
}

bool InputHelper::isHeldLong(Keyboard::Key key) const {
    return timeCurrentlyHeld(key).asMilliseconds() > Single::HOLD_TIME;
}

bool InputHelper::isHeldLong(Mouse::Button button) const {
    return timeCurrentlyHeld(button).asMilliseconds() > Single::HOLD_TIME;
}

bool InputHelper::wasHeldLong(Keyboard::Key key) const {
    return timeHeld(key).asMilliseconds() > Single::HOLD_TIME;
}

bool InputHelper::wasHeldLong(Mouse::Button button) const {
    return timeHeld(button).asMilliseconds() > Single::HOLD_TIME;
}

bool InputHelper::justReleased(Keyboard::Key key) const {
    return keyTime[key].second == currentTime;
}

bool InputHelper::justReleased(Mouse::Button button) const {
    return buttonTime[button].second == currentTime;
}

void InputHelper::queueReleaseEvent(Keyboard::Key key, std::function<void()> func) {
    keyReleaseQueue.emplace(key, func);
}

void InputHelper::queueReleaseEvent(Mouse::Button button, std::function<void()> func) {
    buttonReleaseQueue.emplace(button, func);
}

void InputHelper::queueHeldLongEvent(Keyboard::Key key, std::function<void()> func) {
    keyHeldLongQueue.emplace(key, func);
}

void InputHelper::queueHeldLongEvent(Mouse::Button button, std::function<void()> func) {
    buttonHeldLongQueue.emplace(button, func);
}

Time InputHelper::mouseLastMoved() const {
    return mousePositionInfo.lastMoved;
}

bool InputHelper::hasMouseMovedSince(Time start) const {
    return mouseLastMoved() > start;
}
