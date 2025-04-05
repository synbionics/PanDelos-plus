#ifndef UMBRELLA_ALGO_H_GUARD
#define UMBRELLA_ALGO_H_GUARD

#include "Graph.hh"
#include <unordered_set>
#include <vector>
#include <stack>

// approccio DFS
std::vector<std::vector<int>> connected_components(const Graph& g) {
    std::unordered_set<int> visited;
    std::vector<std::vector<int>> components;

    for (int node = 0; node < g.get_number_of_nodes(); ++node) {
        if (!g.find_node(node) || visited.count(node)) continue;

        std::vector<int> component;
        std::stack<int> stack;
        stack.push(node);

        while (!stack.empty()) {
            int u = stack.top(); stack.pop();

            if (visited.count(u)) continue;
            visited.insert(u);
            component.push_back(u);

            for (const auto& [v, _] : g.get_neighbors(u)) {
                if (!visited.count(v)) {
                    stack.push(v);
                }
            }
        }

        components.push_back(component);
    }

    return components;
}

#endif
