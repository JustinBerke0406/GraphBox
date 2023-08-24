#pragma once

#include <SFML/Graphics.hpp>

template<typename T>
class OptionType {
protected:
    int xCord;
    int yCord;

    T value;

    virtual void updateValueDependents() = 0;
    virtual void onInteract(sf::RenderWindow& window) = 0;
public:
    OptionType<T>(int x, int y, T defaultValue);

    void setValue(T value);
    T getValue() const;

    virtual void draw(sf::RenderWindow& window) = 0;
};

template<typename T>
OptionType<T>::OptionType(int x, int y, T defaultValue) {
    xCord = x;
    yCord = y;

    value = defaultValue;
}

template<typename T>
T OptionType<T>::getValue() const {
    return value;
}

template<typename T>
void OptionType<T>::setValue(T value_) {
    this->value = value_;

    updateValueDependents();
}
