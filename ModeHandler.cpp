#include "ModeHandler.h"

ModeHandler::ModeHandler() {
    for (const auto& key : allSubModes)
        subModes.emplace(key, false);
}

bool& ModeHandler::operator[](std::string key) {
    return subModes[key];
}

void ModeHandler::toggle(std::string key) {
    subModes[key] = !subModes[key];
}

void ModeHandler::clear() {
    mainMode = Mode::Edit;

    for (const auto& key : allSubModes)
        subModes[key] = false;
}

void ModeHandler::mainToggle() {
    if (mainMode == Mode::Connect)
        mainMode = Mode::Edit;
    else
        mainMode = Mode::Connect;
}
