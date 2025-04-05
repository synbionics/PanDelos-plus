#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string.h>
#include <unordered_set>
#include "../lib/Graph.hh"
#include "../lib/Umbrella_algo.hh"
#include "../lib/types.hh"

#ifndef NDEBUG
    #define DEBUG_PRINT(x) std::cout << "[DEBUG] " << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

#ifndef FAST_MODE
#define FAST_MODE 0
#endif

void check_duplicates(const std::unordered_map<int, std::string>& seq_names);
Graph build_graph_from_file(const std::string& file_name);

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cout << "Missing input values" << std::endl;
        return -1;
    }

    std::string seqs_ifile_name = argv[1];
    std::string net_ifile_name = argv[2];

    char sequenceSeparator = '\t';

    std::unordered_map<int, std::string> seq_names;
    std::unordered_map<int, std::string> seq_genome;
    std::unordered_map<int, std::string> seq_descr;
    std::unordered_map<std::string, std::vector<int>> genomes;

    std::ifstream file(seqs_ifile_name);
    std::string line;
    int i = 0, seq_id = 0;
    
    while (getline(file, line)) {
        if (i % 2 == 0) {
            std::stringstream ss(line);
            std::string col0, col1, col2;
            getline(ss, col0, sequenceSeparator);
            getline(ss, col1, sequenceSeparator);
            getline(ss, col2, sequenceSeparator);
            
            seq_names[seq_id] = col1;
            seq_genome[seq_id] = col0;
            seq_descr[seq_id] = col2;
            
            genomes[col0].push_back(seq_id);
            seq_id++;
        }
        i++;
    }

    DEBUG_PRINT("number of sequences: " << seq_names.size());
    DEBUG_PRINT("number of genomes: " << genomes.size());

    #if !FAST_MODE
        DEBUG_PRINT("Checking duplicates");
        check_duplicates(seq_names);
    #endif

    Graph network = build_graph_from_file(net_ifile_name);

    DEBUG_PRINT("number of network nodes: " << network.get_number_of_nodes());
    DEBUG_PRINT("number of network edges: " << network.get_number_of_edges());

    #if !FAST_MODE
        DEBUG_PRINT("Computing connected components");
        DEBUG_PRINT("number of connected components: " << connected_components(network).size());
    #endif
    
    return 0;
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