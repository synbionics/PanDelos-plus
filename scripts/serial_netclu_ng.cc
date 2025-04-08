#include <iostream>
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

        std::map<size_t, node_id_t> comps_size_distr;
        int nof_comps = 0;
        DEBUG_PRINT("----------------------------------------");
        DEBUG_PRINT("Computing connected components...");
        auto components = connected_components(network);

        for(auto& component : components){
            size_t comp_size = component.size();
            ++(comps_size_distr[comp_size]);
            ++nof_comps;
        }

        for (const auto& [size, count] : comps_size_distr)
            DEBUG_PRINT("con dimensione: " << size << " ci sono: " << count << " componenti");

        DEBUG_PRINT("number of connected components: " << nof_comps);
        DEBUG_PRINT("----------------------------------------");

    #endif

    std::unordered_set<int> remaining_singletons;
    for(auto it = seq_names.begin(); it != seq_names.end(); ++it){
        remaining_singletons.insert(it->first);
    }

    // nel file originale fnodes non viene usato
    //std::unordered_set<node_id_t> fnodes;
    // in teoria si fa anche non ordinata quindi più veloce
    // clear non libera memoria -> più veloce, sta in ram
    std::map<size_t, node_id_t> coms_size_distr;
    int nof_coms = 0;

    for(auto& component : connected_components(network)){
        std::cout << "----------------------------------------" << std::endl;
        #if !FAST_MODE
            sort_and_print_component(component, std::cout);
        #endif
        int max_k = get_max_collision(component, network, seq_genome);
        if(max_k > 0){
            std::cout << "max_k: " << max_k << ", coco size: " << component.size() << std::endl;
            DEBUG_PRINT("-*-computing girvan-newman...");
            auto communities = split_until_max_k(component,network, seq_genome);
            nof_coms += communities.size();
            for(auto& community : communities){
                size_t community_length = community.size();
                // da c++98 fino all'attuale inizializza con 0 se non esiste
                coms_size_distr[component.size()] += 1;
                print_family(community, seq_names, std::cout);
                for(const node_id_t& node : community)
                    remaining_singletons.erase(node);
                print_family_descriptions(community, seq_descr, std::cout);
            }
        } else{
            ++nof_coms;
            coms_size_distr[component.size()] += 1;
            print_family(component, seq_names, std::cout);
            for(const node_id_t& node : component)
                    remaining_singletons.erase(node);
            print_family_descriptions(component, seq_descr, std::cout);
        }
    }

    for(const node_id_t& node : remaining_singletons)
        std::cout << "F{" << seq_names.at(node) << " }" << std::endl;

    for (const auto& [k, v] : coms_size_distr)
        std::cout << k << " " << v << std::endl;

    std::cout << "number of communities " << nof_coms << std::endl;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    DEBUG_PRINT("end of net_clu_ng");
    
    return 0;
}