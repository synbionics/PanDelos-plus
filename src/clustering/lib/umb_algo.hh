#ifndef UMBRELLA_ALGO_H_GUARD
#define UMBRELLA_ALGO_H_GUARD

#include "Graph.hh"
#include "SubGraph.hh"
#include "GraphView.hh"
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
#include <mutex>
#include <atomic> 
#include <condition_variable>
#include "UmbrThreadPool.hh"

std::mutex cout_mutex;

//std::ofstream debugFile("kahan_umbr_debug_output.txt");

// importante, rimuovere minmax PER GRAFI DIRETTI

struct Path_info{
    double distance;
    double paths;
    double delta;

    Path_info() : distance(std::numeric_limits<double>::infinity()),
                paths(0), delta(0) {}
};

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

void shortest_paths_bfs(
    const GraphView& g, 
    node_id_t s,
    std::unordered_map<node_id_t, Path_info>& info,
    std::unordered_map<node_id_t, std::vector<node_id_t>>& pred,
    std::stack<node_id_t>& stack
) {
    std::queue<node_id_t> q;
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
}

void shortest_paths_dijkstra(
    const GraphView& g, 
    node_id_t s,
    std::unordered_map<node_id_t, Path_info>& info,
    std::unordered_map<node_id_t, std::vector<node_id_t>>& pred,
    std::stack<node_id_t>& stack
) {
    std::priority_queue<std::pair<double, node_id_t>, 
                       std::vector<std::pair<double, node_id_t>>, 
                       std::greater<std::pair<double, node_id_t>>> pq;
    pq.push({0, s});
    
    while (!pq.empty()) {
        auto [dist, u] = pq.top(); pq.pop();
        
        if (dist > info[u].distance) continue;
        
        stack.push(u);
        
        for (const auto& [v, w] : g.get_neighbors(u)) {
            double alt = info[u].distance + w;
            
            if (info[v].distance > alt) {
                info[v].distance = alt;
                info[v].paths = 0;
                pred[v].clear();
                pq.push({alt, v});
            }
            
            if (std::abs(info[v].distance - alt) < 1e-9) {
                info[v].paths += info[u].paths;
                pred[v].push_back(u);
            }
        }
    }
}

std::unordered_map<std::pair<node_id_t,node_id_t>, weight_t, PairHash>
calculate_edge_betweenness(
    const GraphView& gview,
    UmbrThreadPool& bw_pool,
    const size_t MAX_THREADS,
    const bool THRESHOLD,
    bool is_weighted)
{
    using Edge = std::pair<node_id_t, node_id_t>;

    std::unordered_map<Edge, weight_t, PairHash> edge_betweenness;

    std::vector<node_id_t> nodes(gview.get_nodes().begin(), gview.get_nodes().end());
    std::sort(nodes.begin(), nodes.end());

    auto compute_for_sources = [&](size_t begin_idx, size_t end_idx) {
        std::unordered_map<Edge, weight_t, PairHash> local_edge_betweenness;

        for (size_t idx = begin_idx; idx < end_idx; ++idx) {
            node_id_t s = nodes[idx];

            std::unordered_map<node_id_t, Path_info> info;
            std::unordered_map<node_id_t, std::vector<node_id_t>> pred;
            std::stack<node_id_t> stack;

            for (node_id_t v : nodes) {
                info[v].distance = std::numeric_limits<double>::infinity();
                info[v].paths    = 0;
                info[v].delta    = 0.0;
            }

            info[s].distance = 0.0;
            info[s].paths    = 1;
            info[s].delta    = 0.0;

            if (is_weighted)
                shortest_paths_dijkstra(gview, s, info, pred, stack);
            else
                shortest_paths_bfs(gview, s, info, pred, stack);

            while (!stack.empty()) {
                node_id_t w = stack.top(); stack.pop();

                for (node_id_t v : pred[w]) {
                    // coeff = (sigma[v]/sigma[w]) * (1 + delta[w])
                    double coeff =
                        (static_cast<double>(info[v].paths) / static_cast<double>(info[w].paths)) *
                        (1.0 + info[w].delta);

                    Edge edge = std::minmax(v, w);
                    local_edge_betweenness[edge] += coeff;
                    info[v].delta += coeff;
                }
            }
        }

        return local_edge_betweenness;
    };

    if (THRESHOLD) {
        size_t BATCH = std::max<size_t>(1, (gview.get_number_of_nodes() / MAX_THREADS));

        const size_t n = nodes.size();
        const size_t num_batches = (n + BATCH - 1) / BATCH;

        std::vector<std::unordered_map<Edge, weight_t, PairHash>> per_batch(num_batches);

        for (size_t batch_id = 0; batch_id < num_batches; ++batch_id) {
            const size_t begin_idx = batch_id * BATCH;

            bw_pool.execute([&, batch_id, begin_idx] {
                const size_t end_idx = std::min(begin_idx + BATCH, nodes.size());
                per_batch[batch_id] = compute_for_sources(begin_idx, end_idx);
            });
        }

        bw_pool.wait();

        std::unordered_map<Edge, double, PairHash> comp; // compensazione Kahan

        for (size_t batch_id = 0; batch_id < num_batches; ++batch_id) {
            for (const auto& [edge, val] : per_batch[batch_id]) {
                double& sum = edge_betweenness[edge];
                double& c   = comp[edge];

                double y = static_cast<double>(val) - c;
                double t = static_cast<double>(sum) + y;
                c = (t - static_cast<double>(sum)) - y;
                sum = static_cast<weight_t>(t);
            }
        }
    } else {
        edge_betweenness = compute_for_sources(0, nodes.size());
    }

    for (auto& [edge, val] : edge_betweenness)
        val = static_cast<weight_t>(static_cast<double>(val) / 2.0);

    return edge_betweenness;
}


bool is_inconsistent(std::vector<node_id_t> component, const Graph& network,
    const std::unordered_map<node_id_t, std::string>& seq_genome){

    std::unordered_map<std::string, std::vector<node_id_t>> collisions;

    for(const node_id_t& node : component){
        try {
            std::string genome = seq_genome.at(node);
            collisions[genome].push_back(node);
        } catch (const std::out_of_range& e) {
            std::cerr << "[is_inconsistent] Key not found in seq_genome: " << node << std::endl;
            throw;
        }
    }

    for(auto& [genome, nodes] : collisions){
        for(size_t i=0; i<nodes.size(); ++i){
            node_id_t node_1 = nodes[i];

            for(size_t j=0; j<nodes.size(); ++j){
                node_id_t node_2 = nodes[j];

                if(nodes[i] != nodes[j] && !network.exists_edge(node_1,node_2))
                    return true;
            }
        }
    }

    // la componente è consistente
    return false;
}

bool is_inconsistent(std::vector<node_id_t> component, const GraphView& network,
    const std::unordered_map<node_id_t, std::string>& seq_genome){

    std::unordered_map<std::string, std::vector<node_id_t>> collisions;

    for(const node_id_t& node : component){
        try {
            std::string genome = seq_genome.at(node);
            collisions[genome].push_back(node);
        } catch (const std::out_of_range& e) {
            std::cerr << "[is_inconsistent] Key not found in seq_genome: " << node << std::endl;
            throw;
        }
    }

    int max_k = 0;

    for(auto& [genome, nodes] : collisions){
        for(size_t i=0; i<nodes.size(); ++i){
            node_id_t node_1 = nodes[i];

            for(size_t j=0; j<nodes.size(); ++j){
                node_id_t node_2 = nodes[j];

                if(nodes[i] != nodes[j] && !network.exists_edge(node_1,node_2))
                    return true;
            }
        }
    }

    // la componente è consistente
    return false;
}

std::pair<node_id_t, node_id_t> calculate_heaviest(const GraphView& network, const std::unordered_map<std::pair<node_id_t, node_id_t>, weight_t, PairHash>& edge_bws_map){

    weight_t current_max = -1;
    const double EPSILON = 1e-6;
    std::pair<node_id_t,node_id_t> max_bw_edge = std::make_pair(0,0);

    for(auto it = edge_bws_map.begin(); it != edge_bws_map.end(); ++it){
        //std::lock_guard<std::mutex> lock(cout_mutex);
        auto& current_betweeness = it->second;
        
        //debugFile << "attuale max_current: " << current_max << std::endl;
        //debugFile << "valore del bw esaminato tra i nodi " << it->first.first << " e " << it->first.second << ": " << current_betweeness << std::endl;
        
        if(current_betweeness > current_max){
            current_max = current_betweeness;
            max_bw_edge = it->first;
        }
        else if(std::abs(current_betweeness - current_max) < EPSILON){
            //debugFile << "\n!!! betweeness uguale trovato e valente: " << current_betweeness << " !!!\n";
            weight_t current_edge_weight = network.get_edge_weight(it->first.first, it->first.second);
            weight_t max_bw_edge_weight = network.get_edge_weight(max_bw_edge.first, max_bw_edge.second);
            //debugFile << "\nVecchio arco candidato fra " << max_bw_edge.first << " e " << max_bw_edge.second << std::endl;
            
            if(current_edge_weight < max_bw_edge_weight){
                max_bw_edge = it->first;
            }
            // Terzo criterio: confronto lessicografico completo della coppia
            else if(std::abs(current_edge_weight - max_bw_edge_weight) < EPSILON){
                // Confronta (first, second) in ordine lessicografico
                if(it->first < max_bw_edge){  // std::pair ha operator< già definito!
                    max_bw_edge = it->first;
                }
            }
            //debugFile << "Nuovo arco candidato fra " << max_bw_edge.first << " e " << max_bw_edge.second << std::endl;
        }    
    }

    //debugFile << "!!!ARCO che tagliero' e' fra " << max_bw_edge.first << " e " << max_bw_edge.second << std::endl;

    return max_bw_edge;
}

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

std::vector<std::vector<node_id_t>> connected_components(const GraphView& g) {
    std::vector<std::vector<node_id_t>> components;
    std::unordered_set<node_id_t> visited;
    auto nodes = g.get_nodes();
    
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

std::vector<std::vector<node_id_t>> girvan_newman(GraphView& network, UmbrThreadPool& pool, const size_t MAX_THREADS, const bool THRESHOLD, bool is_weighted){
    
    while(connected_components(network).size() <= 1){
        const auto& edge_bws = calculate_edge_betweenness(network, pool, MAX_THREADS, THRESHOLD, is_weighted);
        auto heaviest_edge = calculate_heaviest(network, edge_bws);
        network.remove_edge(heaviest_edge);
    }
    return connected_components(network);
}

std::vector<std::vector<node_id_t>> split_until_max_k(
                const std::vector<node_id_t>& component,
                Graph& network, const std::unordered_map<int, std::string>& seq_genome,
                UmbrThreadPool& pool,
                const size_t MAX_THREADS,
                const bool THRESHOLD,
                bool is_weighted = false)
{
    GraphView component_subnet_view = GraphView(network, component);

    std::vector<std::vector<node_id_t>> tmp_communities = girvan_newman(component_subnet_view,pool,MAX_THREADS,THRESHOLD,is_weighted);
    std::vector<std::vector<node_id_t>> final_communities;

    std::vector<std::vector<node_id_t>> to_process(tmp_communities.begin(), tmp_communities.end());

    while (!to_process.empty()) {
        std::vector<node_id_t> community = to_process.back();
        to_process.pop_back();

        if (is_inconsistent(community, component_subnet_view, seq_genome)) {
            std::vector<std::vector<node_id_t>> subresult = split_until_max_k(community, network, seq_genome, pool, MAX_THREADS, THRESHOLD, is_weighted);
            final_communities.insert(final_communities.end(), subresult.begin(), subresult.end());
        } else {
            final_communities.push_back(community);
        }
    }

    return final_communities;
}

void sort_and_print_component(std::vector<node_id_t>& component, std::ostream& out_op) {

    std::sort(component.begin(), component.end());

    std::lock_guard<std::mutex> lock(cout_mutex);
    out_op << "coco: [";

    for (size_t i = 0; i < component.size(); ++i) {
        out_op << component[i];
        if (i != component.size() - 1)
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
            //std::cout << "Duplicated seq name: " << pair.second << std::endl;
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