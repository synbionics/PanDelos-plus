#include "Graph.h"
#include <iostream>

void Graph::addNode(int u){
    nodes.insert(u);
}

// non controlla esistenza effettiva dei nodi
void Graph::addEdge(int u, int v, double w) {
    adj[u].push_back({v, w});
    adj[v].push_back({u, w});
    ++number_of_edges;
}

void Graph::printGraph() const {
    for (const auto& [node, neighbors] : adj) {
        std::cout << node << " -> ";
        for (const auto& [neighbor, weight] : neighbors) {
            std::cout << "(" << neighbor << ", " << weight << ") ";
        }
        std::cout << std::endl;
    }
}

bool Graph::find_node(int u) const{
    if(nodes.find(u) == nodes.end())
        return false;
    else return true;
}

int Graph::get_number_of_nodes() const{
    return nodes.size();
}

int Graph::get_number_of_edges() const{
    return number_of_edges;;
}

const std::vector<std::pair<int, double>>& Graph::get_neighbors(int u) const {
    static const std::vector<std::pair<int, double>> empty;
    auto it = adj.find(u);
    if (it != adj.end()) {
        return it->second;
    }
    return empty;
}
