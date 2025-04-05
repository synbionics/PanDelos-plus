#ifndef UMBRELLA_ALGO_H_GUARD
#define UMBRELLA_ALGO_H_GUARD

#include "Graph.hh"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>

int get_max_collision(std::vector<node_id_t> component, const Graph& network,
    const std::unordered_map<node_id_t, std::string>& seq_genome){

    std::unordered_map<std::string, std::vector<node_id_t>> collisions;

    for(const node_id_t& node : component){
        std::string genome = seq_genome.at(node);
        collisions[genome].push_back(node);
    }

    int max_k = 0;

    for(auto& [genome, nodes] : collisions){
        for(size_t i=0; i<nodes.size(); ++i){
            node_id_t node_1 = nodes[i];
            int s_k = 0;

            for(size_t j=0; j<nodes.size(); ++j){
                node_id_t node_2 = nodes[j];

                if(nodes[i] != nodes[j] && !network.exists_edge(node_1,node_2))
                    ++s_k;
                
            }
            max_k = std::max(max_k,s_k);
        }
    }

    return max_k;

}

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
