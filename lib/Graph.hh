#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <iostream>

using node_id_t = int;
using weight_t = double;

class Graph {
public:
    void addNode(node_id_t u) {
        nodes.insert(u);
    }

    void addEdge(node_id_t u, node_id_t v, double w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
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

    bool find_node(node_id_t u) const {
        return nodes.find(u) != nodes.end();
    }

    int get_number_of_nodes() const {
        return nodes.size();
    }

    bool exists_edge(node_id_t u, node_id_t v) const{
        const auto& neighbors = this->get_neighbors(u);

        for (const auto& [neighbor, weight] : neighbors) {
            if (neighbor == v) 
                return true;
        }

        return false;
    }

    int get_number_of_edges() const {
        return number_of_edges;
    }

    const std::vector<std::pair<node_id_t, weight_t>>& get_neighbors(node_id_t u) const {
        static const std::vector<std::pair<node_id_t, double>> empty;
        auto it = adj.find(u);
        if (it != adj.end()) {
            return it->second;
        }
        return empty;
    }

private:
    std::unordered_set<node_id_t> nodes;
    std::unordered_map<node_id_t, std::vector<std::pair<node_id_t, weight_t>>> adj;
    int number_of_edges = 0;
};

#endif
