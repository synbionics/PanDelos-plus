#ifndef GRAPH_H_GUARD
#define GRAPH_H_GUARD

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <iostream>

class Graph {
public:
    void addNode(int u) {
        nodes.insert(u);
    }

    void addEdge(int u, int v, double w) {
        adj[u].emplace_back(v, w);
        adj[v].emplace_back(u, w);
        ++number_of_edges;
    }

    void printGraph() const {
        for (const auto& [node, neighbors] : adj) {
            std::cout << node << " -> ";
            for (const auto& [neighbor, weight] : neighbors) {
                std::cout << "(" << neighbor << ", " << weight << ") ";
            }
            std::cout << std::endl;
        }
    }

    bool find_node(int u) const {
        return nodes.find(u) != nodes.end();
    }

    int get_number_of_nodes() const {
        return nodes.size();
    }

    int get_number_of_edges() const {
        return number_of_edges;
    }

    const std::vector<std::pair<int, double>>& get_neighbors(int u) const {
        static const std::vector<std::pair<int, double>> empty;
        auto it = adj.find(u);
        return it != adj.end() ? it->second : empty;
    }

private:
    std::unordered_set<int> nodes;
    std::unordered_map<int, std::vector<std::pair<int, double>>> adj;
    int number_of_edges = 0;
};

#endif
