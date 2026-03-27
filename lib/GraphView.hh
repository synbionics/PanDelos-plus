#ifndef GRAPHVIEW_GUARD
#define GRAPHVIEW_GUARD

#include "Graph.hh"
#include <unordered_set>
#include <vector>
#include <algorithm>

class GraphView {
    Graph& g;
    std::unordered_set<node_id_t> visible_nodes;

public:
    GraphView(Graph& graph, const std::vector<node_id_t>& subset)
        : g(graph), visible_nodes(subset.begin(), subset.end()) {}

    bool has_node(node_id_t node) const { 
        return visible_nodes.count(node) > 0;
    }

    const std::vector<std::pair<node_id_t, weight_t>> &get_neighbors(node_id_t node) const {
        return g.get_neighbors(node);
    }

    void remove_edge(const std::pair<node_id_t, node_id_t>& edge) {
        g.remove_edge(edge);
    }

    bool exists_edge(node_id_t u, node_id_t v) const {
        auto neighbors = get_neighbors(u);
        return std::find_if(neighbors.begin(), neighbors.end(), 
                        [v](const auto& neighbor) { return neighbor.first == v; }) 
            != neighbors.end();
    }

    weight_t get_edge_weight(node_id_t u, node_id_t v) const {
        return g.get_edge_weight(u,v);
    }

    const size_t get_number_of_nodes() const {
        return get_nodes().size();
    }

    const std::unordered_set<node_id_t>& get_nodes() const { return visible_nodes; }
};

#endif
