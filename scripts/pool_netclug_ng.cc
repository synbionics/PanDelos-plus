#include <iostream>
#include <unordered_map>
#include <vector>
#include <string.h>
#include <unordered_set>
#include "../lib/Graph.hh"
#include "../lib/Umbrella_algo.hh"
#include "../lib/types.hh"

#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

#include "../lib/UmbrThreadPool.hh"

const size_t MAX_THREADS = std::thread::hardware_concurrency();

std::mutex remaining_singletons_mutex;

#ifndef NDEBUG
    #define DEBUG_PRINT(x) std::cout << "[DEBUG] " << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

#ifndef FAST_MODE
#define FAST_MODE 0
#endif

void process_component(std::vector<node_id_t> component,
                       Graph& network,
                       const std::unordered_map<int, std::string>& seq_genome,
                       const std::unordered_map<int, std::string>& seq_names,
                       const std::unordered_map<int, std::string>& seq_descr,
                       std::atomic_int& nof_coms,
                       std::map<size_t, node_id_t>& coms_size_distr,
                       size_t component_size,
                       std::unordered_set<int>& remaining_singletons
                       ){

    std::vector<std::vector<node_id_t>> communities =  split_until_max_k(component, network, seq_genome);
    nof_coms += communities.size();
    
    for(auto& community : communities){
        coms_size_distr[component_size] += 1;
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            print_family(community, seq_names, std::cout);
            print_family_descriptions(community, seq_descr, std::cout);
        }
        std::lock_guard rs_lock(remaining_singletons_mutex);
        for(const node_id_t& node : community)
            remaining_singletons.erase(node);
    }
}

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
    std::atomic<int> nof_coms = 0;

    UmbrThreadPool pool(MAX_THREADS);

    for(auto component : connected_components(network)){

        //std::cout << "----------------------------------------" << std::endl;
        #if !FAST_MODE
            sort_and_print_component(component, std::cout);
        #endif
        int max_k = get_max_collision(component, network, seq_genome);
        if(max_k > 0){

            size_t component_size = component.size();

            pool.execute(
                process_component,
                component,
                std::ref(network),
                std::cref(seq_genome),
                std::cref(seq_names),
                std::cref(seq_descr),
                std::ref(nof_coms),
                std::ref(coms_size_distr),
                component_size,
                std::ref(remaining_singletons)
            );

            //std::cout << "max_k: " << max_k << ", coco size: " << component.size() << std::endl;

        } else{
            ++nof_coms;
            coms_size_distr[component.size()] += 1;
            {
                std::lock_guard rs_lock(remaining_singletons_mutex);
                for(const node_id_t& node : component)
                remaining_singletons.erase(node);
            }
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                print_family(component, seq_names, std::cout);
                print_family_descriptions(component, seq_descr, std::cout);
            }
        }
    }

//GCA_000200735.1:FR773153.2:HF1_11170:1
// !!! problema classico
// devo eseguirlo alla fine dopo tutti i thread, NON nel mentre o stampa quelli che ancora non sono stati rimossi da remaining_singletons
pool.wait();


{
    
    std::lock_guard<std::mutex> lock(cout_mutex);
    DEBUG_PRINT("\nfinito lavoro parallelo\n");
    for(const node_id_t& node : remaining_singletons)
        std::cout << "F{ " << seq_names.at(node) << " }" << std::endl;

    for (const auto& [k, v] : coms_size_distr)
        std::cout << k << " " << v << std::endl;

    std::cout << "number of communities " << nof_coms << std::endl;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
    DEBUG_PRINT("end of net_clu_ng");
    
    return 0;
}
