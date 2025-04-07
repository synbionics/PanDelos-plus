#ifndef UMBRELLA_ALGO_H_GUARD
#define UMBRELLA_ALGO_H_GUARD

#include "Graph.hh"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>
#include <limits>
#include <map>
#include <queue>
#include <algorithm>
#include <set>
#include <sstream>
#include <fstream>

struct Path_info{
    double distance;
    double paths;
    double delta;

    Path_info() : distance(std::numeric_limits<double>::infinity()),
                paths(0), delta(0) {}
};

struct PairHash {
        template <class T1, class T2>
        std::size_t operator() (const std::pair<T1, T2>& pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

std::unordered_map<std::pair<node_id_t,node_id_t>, weight_t, PairHash> calculate_edge_betweenness(const Graph& g) {

    std::unordered_map<std::pair<node_id_t, node_id_t>, weight_t, PairHash> edge_betweenness;

    for (node_id_t s : g.get_nodes()) {
        std::unordered_map<node_id_t, Path_info> info;
        std::unordered_map<node_id_t, std::vector<node_id_t>> pred;
        std::queue<node_id_t> q;
        std::stack<node_id_t> stack;

        info[s].distance = 0;
        info[s].paths = 1;

        //BFS che fa anche da shortest paths (valido solo per grafo non pesato)

        q.push(s);
        while (!q.empty()) {
            node_id_t u = q.front(); q.pop();
            stack.push(u);

            for (const auto& [v, _] : g.get_neighbors(u)) {
                
                if (info[v].distance == std::numeric_limits<double>::infinity()) {
                    info[v].distance = info[u].distance + 1;
                    q.push(v);
                }

                if (info[v].distance == info[u].distance + 1) {
                    info[v].paths += info[u].paths;
                    pred[v].push_back(u);
                }
            }
        }

        while (!stack.empty()) {
            node_id_t w = stack.top(); stack.pop();
            for (node_id_t v : pred[w]) {
                double coefficent = (info[v].paths / info[w].paths) * (1 + info[w].delta);
                //considero l'arco come dall'id più piccolo al più grande
                std::pair<node_id_t, node_id_t> edge = std::minmax(v, w);
                edge_betweenness[edge] += coefficent;
                info[v].delta += coefficent;
            }
        }
    }

    // opzionale? alla fine è O(m) ma a me interessa solo il più grande
    for (auto& [edge, val] : edge_betweenness) {
        val /= 2.0;
    }

    return edge_betweenness;
}

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

std::pair<node_id_t, node_id_t> calculate_heaviest(const std::unordered_map<std::pair<node_id_t, node_id_t>, weight_t, PairHash>& edge_bws_map){

    weight_t current_max = 0;
    std::pair<node_id_t,node_id_t>max_bw_edge = std::make_pair(0,0);

    for(auto it = edge_bws_map.begin(); it != edge_bws_map.end(); ++it){
        auto& current_betweeness = it->second;
        if(current_betweeness > current_max){
            current_max = current_betweeness;
            max_bw_edge = it->first;
        }    
    }

    return max_bw_edge;

}

// approccio DFS
std::vector<std::vector<node_id_t>> connected_components(const Graph& g) {
    std::vector<std::vector<node_id_t>> components;
    std::unordered_set<node_id_t> visited;
    std::vector<node_id_t> nodes = g.get_nodes();
    
    for (const auto& node : nodes) {
        if (visited.find(node) == visited.end()) {
            std::vector<node_id_t> component;
            std::vector<node_id_t> stack;
            
            stack.push_back(node);
            visited.insert(node);
            
            while (!stack.empty()) {
                node_id_t current = stack.back();
                stack.pop_back();
                component.push_back(current);
                
                for (const auto& [neighbor, _] : g.get_neighbors(current)) {
                    if (visited.find(neighbor) == visited.end()) {
                        stack.push_back(neighbor);
                        visited.insert(neighbor);
                    }
                }
            }
            
            components.push_back(component);
        }
    }
    
    return components;
}

std::vector<std::vector<node_id_t>> single_split_girvan_newman(Graph& network){

    const auto& edge_bws = calculate_edge_betweenness(network);
    auto heaviest_edge = calculate_heaviest(edge_bws);
    network.remove_edge(heaviest_edge);
    return connected_components(network);

}

std::vector<std::vector<node_id_t>> split_until_max_k(
                const std::vector<node_id_t>& component,
                const Graph& network, const std::unordered_map<int, std::string>& seq_genome)
{
    Graph component_subnet = network.subgraph(component);
    
    int max_collision = get_max_collision(component, network, seq_genome);
    
    if (max_collision <= 0) {
        return {component};
    }
    
    std::vector<std::vector<node_id_t>> tmp_communities = single_split_girvan_newman(component_subnet);
    
    if (tmp_communities.size() <= 1) {
        return {component};
    }
    
    std::vector<std::vector<node_id_t>> final_communities;
    
    for (const auto& community : tmp_communities) {
        if (community.size() < component.size()) {
            auto subresult = split_until_max_k(community, network, seq_genome);
            final_communities.insert(final_communities.end(), subresult.begin(), subresult.end());
        } else {
            final_communities.push_back(community);
        }
    }
    
    return final_communities;
}

void sort_and_print_component(const std::vector<node_id_t>& component, std::ostream& out_op) {
    out_op << "coco: [";

    std::vector<node_id_t> sorted_component = component;
    std::sort(sorted_component.begin(), sorted_component.end());

    for (size_t i = 0; i < sorted_component.size(); ++i) {
        out_op << sorted_component[i];
        if (i != sorted_component.size() - 1)
            out_op << ", ";
    }

    out_op << "]" << std::endl;
}

void print_family(const std::vector<node_id_t>& community, const std::unordered_map<node_id_t, std::string>& seq_names, std::ostream& out_op) {
    out_op << "dimensione community: " << community.size() << std::endl;

    out_op << "fam [";
    std::vector<node_id_t> sorted_community = community;
    std::sort(sorted_community.begin(), sorted_community.end());
    for (size_t i = 0; i < sorted_community.size(); ++i) {
        out_op << sorted_community[i];
        if (i != sorted_community.size() - 1)
            out_op << ", ";
    }
    out_op << "]" << std::endl;

    out_op << "F{ ";
    for (size_t i = 0; i < sorted_community.size(); ++i) {
        out_op << seq_names.at(sorted_community[i]);
        if (i != sorted_community.size() - 1)
            out_op << " ; ";
    }
    out_op << "}" << std::endl;
}

void print_family_descriptions(const std::vector<node_id_t>& community, const std::unordered_map<node_id_t, std::string>& seq_descr, std::ostream& out_op) {
    out_op << "D{ ";
    for (size_t i = 0; i < community.size(); ++i) {
        out_op << seq_descr.at(community[i]);
        if (i != community.size() - 1)
            out_op << " ; ";
    }
    out_op << "}" << std::endl;

    out_op << "S{ ";
    std::set<std::string> unique_descriptions;
    for (const auto& node : community) {
        unique_descriptions.insert(seq_descr.at(node));
    }

    bool first = true;
    for (const auto& desc : unique_descriptions) {
        if (!first) {
            out_op << " ; ";
        }
        out_op << desc;
        first = false;
    }
    out_op << "}" << std::endl;

    out_op << "-" << std::endl;
}



void check_duplicates(const std::unordered_map<int, std::string>& seq_names) {
    std::unordered_map<std::string, int> name_count;

    for (const auto& pair : seq_names) {
        name_count[pair.second]++;
        if (name_count[pair.second] > 1) {
            std::cout << "Duplicated seq name: " << pair.second << std::endl;
        }
    }
}

Graph build_graph_from_file(const std::string& file_name){
    char netSeparator = ',';

    Graph graph;
    std::ifstream file(file_name);
    std::string line;
    
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string col0, col1, col2;
        getline(ss, col0, netSeparator);
        getline(ss, col1, netSeparator);
        getline(ss, col2, netSeparator);

        if(std::stof(col2) != 0.0){
            int node_0 = std::stoi(col0);
            int node_1 = std::stoi(col1);
            float edge_weight = std::stof(col2);
            if(!graph.find_node(node_0)){
                graph.addNode(node_0);
            }
            if(node_0 != node_1 && !graph.find_node(node_1)){
                graph.addNode(node_1);
            }
            if(node_0 != node_1){
                graph.addEdge(node_0,node_1,edge_weight);
            }
        }

    }

    return graph;

}

#endif
