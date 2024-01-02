#include "FileManager.h"

void FileManager::save(std::string fileName) {
    Single& single = Single::instance();

    std::ofstream file(fileName);

    GraphState* state = single.state;

    int nodeNum = state->nodeCount();

    file << nodeNum << "|";

    auto nodes = state->getNodes();

    auto nodeIndex = [nodes](Node* node) {
        auto res = std::find(nodes.begin(), nodes.end(), node);

        if (res != nodes.end())
            return (int) (res - nodes.begin());
        else
            return -1;
    };

    for (int i = 0; i < nodeNum; i++) {
        auto node = nodes[i];
        int connections = node->connections.size();

        file << node->label << "|";
        file << node->x << "|" << node->y << "|";
        file << connections << "|";

        for (int j = 0; j < connections; j++)
            file << nodeIndex(node->connections[j]) << "|";
    }

    if (single.mode["directed"])
        file << "1|";
    else
        file << "0|";

    file.close();

    single.window.setTitle(getFileName(fileName) + " - GraphBox");

    single.fileName = fileName;

    std::filesystem::current_path("../");
}

void FileManager::load(std::string fileName) {
    Single& single = Single::instance();

    GraphState* state = single.state;

    std::ifstream file(fileName);
    std::string data;

    std::getline(file, data);

    state->reset();

    auto pullData = [&data]() {
        int prevDem = data.find('|');
        std::string token = data.substr(0, prevDem);

        if (prevDem != data.size() - 1)
            data = data.substr(prevDem+1);

        return token;
    };

    int nodeCount = std::stoi(pullData());

    std::vector<Node*>& nodes = state->getNodes();

    for (int i = 0; i < nodeCount; i++) {
        nodes.push_back(new Node());
    }

    for (int i = 0; i < nodeCount; i++) {
        Node* node = nodes[i];

        std::string label = pullData();

        float x = std::stof(pullData());
        float y = std::stof(pullData());

        int connections = std::stoi(pullData());

        node->label = label;
        node->x = x;
        node->y = y;

        for (int k = 0; k < connections; k++)
            node->connections.push_back(nodes[std::stoi(pullData())]);
    }

    single.mode["directed"] = pullData() == "1";

    file.close();

    single.window.setTitle(getFileName(fileName) + " - GraphBox");

    single.fileName = fileName;

    std::filesystem::current_path("../");
}

void FileManager::saveDialog() {
    Single& single = Single::instance();

    HANDLE hf;              // file handle

    auto& ofn = single.ofn;

    ofn.lpstrFile = single.szFile;

    if (GetSaveFileName(&ofn) == TRUE) {
        hf = CreateFile(ofn.lpstrFile,
                        GENERIC_READ,
                        0,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

        CloseHandle(hf);

        FileManager::save(single.szFile);
    }
}

void FileManager::loadDialog() {
    Single& single = Single::instance();

    HANDLE hf;              // file handle

    auto& ofn = single.ofn;
    ofn.lpstrFile[0] = '\0';

    if (GetOpenFileName(&ofn) == TRUE) {
        hf = CreateFile(ofn.lpstrFile,
                        GENERIC_READ,
                        0,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

        CloseHandle(hf);

        FileManager::load(single.szFile);
    }
}

std::string FileManager::getFileName(std::string path) {
    std::string wExt = path.substr(path.find_last_of('\\')+1);

    return wExt.substr(0, wExt.size()-6);
}
