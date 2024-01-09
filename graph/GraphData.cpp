#include "GraphData.h"

#include "../Single.h"

void GraphData::updateDegrees() {
    Single& single = Single::instance();
    GraphState* state = single.state;

    std::map<Node*, int> container;
    std::vector<std::pair<Node*, int>> sorter;

    degree.clear();

    for (Node* node : state->getNodes()) {
        container[node] += node->connections.size();

        if (!single.mode["directed"])
            for (Node* adj : node->connections)
                if (!adj->directlyConnectsTo(node))
                    container[adj]++;
    }

    for (auto value : container)
        sorter.emplace_back(value);

    std::sort(sorter.begin(), sorter.end(), [] (auto lhs, auto rhs) -> bool {return lhs.second < rhs.second;});

    for (auto value : sorter) {
        degree.emplace_back(value.first);
        value.first->degrees = value.second;
    }
}

int GraphData::uniqueDegrees() {
    int prev = -1, count = 0;

    for (Node* node : degree) {
        if (node->degrees > prev) {
            prev = node->degrees;
            count++;
        }
    }

    return count;
}
