#include "Graph.hh"

class SubGraph : public Graph {
private:
    Graph& original_graph;
    std::unordered_set<node_id_t> nodes_subset;
    
public:
    SubGraph(Graph& orig, const std::vector<node_id_t>& subset) 
        : original_graph(orig) {
        for (const node_id_t& node : subset) {
            nodes_subset.insert(node);
            addNode(node);
        }
        
        for (const node_id_t& node : subset) {
            if (original_graph.has_node(node)) {
                for (const auto& neighbor_pair : original_graph.get_neighbors(node)) {
                    node_id_t neighbor = neighbor_pair.first;
                    weight_t weight = neighbor_pair.second;
                    
                    if (nodes_subset.find(neighbor) != nodes_subset.end()) {
                        addEdge(node, neighbor, weight);
                    }
                }
            }
        }
    }
    
    void remove_edge(const std::pair<node_id_t, node_id_t>& edge) override {
        Graph::remove_edge(edge);
        original_graph.remove_edge(edge);
    }
    
    // Sovrascrivi altri metodi di modifica del grafo per propagare le modifiche
    void addEdge(node_id_t from, node_id_t to, weight_t weight) override {
        Graph::addEdge(from, to, weight);
        if (nodes_subset.find(from) != nodes_subset.end() && 
            nodes_subset.find(to) != nodes_subset.end()) {
            original_graph.addEdge(from, to, weight);
        }
    }

    SubGraph subgraph(const std::vector<node_id_t>& nodes_subset) {
        return SubGraph(*this, nodes_subset);
    }

};