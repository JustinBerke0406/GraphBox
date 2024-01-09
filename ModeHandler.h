#pragma once

#include <map>
#include <string>
#include <vector>

class ModeHandler {
    std::map<std::string, bool> subModes;

    const std::vector<std::string> allSubModes = {"directed", "error", "force", "inv", "adj", "opt", "type", "mask"};
public:
    ModeHandler();

    enum Mode { Edit, Connect};

    Mode mainMode = Mode::Edit;

    bool& operator [](std::string key);

    void toggle(std::string key);

    void clear();

    void mainToggle();
};