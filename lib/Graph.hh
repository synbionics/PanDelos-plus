#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <iostream>
#include <cassert>
#include <algorithm>

using node_id_t = int;
using weight_t = double;

class Graph {
public:
    void addNode(node_id_t u) {
        nodes.insert(u);
    }

    std::vector<node_id_t> get_nodes() const {
        return std::vector<node_id_t>(nodes.begin(), nodes.end());
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
        return nodes.count(u) > 0;
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

    virtual void addEdge(node_id_t u, node_id_t v, weight_t weight) {
        if (!exists_edge(u, v)) {
            adj[u].emplace_back(v, weight);
            adj[v].emplace_back(u, weight); // non direzionale
            ++number_of_edges;
        }
    }

    weight_t get_edge_weight(node_id_t u, node_id_t v) const {
        auto it = adj.find(u);

        for (const auto& [neighbor, weight] : it->second) {
            if (neighbor == v) {
                return weight;
            }
        }

        //TODO gestione errori? rallenta pero'
        return -1;
    }

    int get_number_of_edges() const {
        return number_of_edges;
    }

    //edge è in forma minmax
    virtual void remove_edge(const std::pair<node_id_t, node_id_t>& edge) {
    node_id_t node_1 = edge.first;
    node_id_t node_2 = edge.second;

    if (!exists_edge(node_1, node_2)) {
        std::cerr << "Attenzione: arco inesistente tra " << node_1 << " e " << node_2 << std::endl;
        return;
    }

    if (adj.find(node_1) == adj.end() || adj.find(node_2) == adj.end()) {
        std::cerr << "Errore: uno dei nodi non esiste in adj.\n";
        return;
    }

    auto& node_1_adj_vec = adj[node_1];
    node_1_adj_vec.erase(
        std::remove_if(node_1_adj_vec.begin(), node_1_adj_vec.end(),
                       [node_2](const auto& pair) { return pair.first == node_2; }),
        node_1_adj_vec.end());

    auto& node_2_adj_vec = adj[node_2];
    node_2_adj_vec.erase(
        std::remove_if(node_2_adj_vec.begin(), node_2_adj_vec.end(),
                       [node_1](const auto& pair) { return pair.first == node_1; }),
        node_2_adj_vec.end());

    --number_of_edges;

    //std::cout << "Ho rimosso l'arco (non direzionale) fra i due nodi: "
    //          << node_1 << " e " << node_2 << std::endl;
}


    bool has_node(node_id_t u) const{
        return nodes.count(u) > 0;
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
    std::set<node_id_t> nodes;
    std::unordered_map<node_id_t, std::vector<std::pair<node_id_t, weight_t>>> adj;
    int number_of_edges = 0;
};

#endif
