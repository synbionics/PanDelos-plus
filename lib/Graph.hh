#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <unordered_set>
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

    int get_number_of_edges() const {
        return number_of_edges;
    }

    //edge è in forma minmax
    virtual void remove_edge(const std::pair<node_id_t, node_id_t>& edge) {

        // magari metto fast mode
        assert(exists_edge(edge.first, edge.second));

        node_id_t node_1 = edge.first;
        node_id_t node_2 = edge.second;

        auto& node_1_adj_vec = adj[node_1];
        for (auto it = node_1_adj_vec.begin(); it != node_1_adj_vec.end(); ++it) {
            if (it->first == node_2) {
                node_1_adj_vec.erase(it);
                std::cout << "ho rimosso l'arco (non direzionale), fra i due nodi: " << node_1 << " e " << node_2 << std::endl;
                break;
            }
        }

        auto& node_2_adj_vec = adj[node_2];
        for (auto it = node_2_adj_vec.begin(); it != node_2_adj_vec.end(); ++it) {
            if (it->first == node_1) {
                node_2_adj_vec.erase(it);
                break;
            }
        }

        --number_of_edges;
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

    Graph subgraph(const std::vector<node_id_t>& nodes_subset) const{
        Graph subgraph;

        for (const node_id_t& node : nodes_subset) {
            subgraph.addNode(node);
        }

        for (const node_id_t& node : nodes_subset) {
            if (adj.find(node) != adj.end()) {
                for (const auto& neighbor_pair : adj.at(node)) {
                    node_id_t neighbor = neighbor_pair.first;
                    weight_t weight = neighbor_pair.second;
                    // ottimizzabile mettendo il controllo da altre parti,
                    // potrei qui inserire e basta le liste di adiacenza anche se alcuni nodi non appaiono
                    if (std::find(nodes_subset.begin(), nodes_subset.end(), neighbor) != nodes_subset.end()) {
                        subgraph.addEdge(node, neighbor, weight);
                    }
                }
            }
        }

        return subgraph;

    }

private:
    std::unordered_set<node_id_t> nodes;
    std::unordered_map<node_id_t, std::vector<std::pair<node_id_t, weight_t>>> adj;
    int number_of_edges = 0;
};

#endif
