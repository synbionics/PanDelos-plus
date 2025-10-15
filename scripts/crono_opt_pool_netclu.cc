#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <map>
#include <chrono>
#include "../lib/Graph.hh"
#include "../lib/Umbrella_algo.hh"
#include "../lib/types.hh"
#include "../lib/UmbrThreadPool.hh"

const size_t MAX_THREADS = std::thread::hardware_concurrency();

std::mutex data_mutex;              // per coms_size_distr e remaining_singletons

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
                       ) {

    std::stringstream oss;
    std::map<size_t, node_id_t> local_coms_size;
    std::vector<node_id_t> local_removed;

    std::vector<std::vector<node_id_t>> communities = split_until_max_k(component, network, seq_genome);
    nof_coms += static_cast<int>(communities.size());

    for (auto &community : communities) {
        local_coms_size[component_size] += 1;
        for (const node_id_t &n : community) local_removed.push_back(n);

        print_family(community, seq_names, oss);
        print_family_descriptions(community, seq_descr, oss);
    }

    // merge
    {
        std::lock_guard<std::mutex> data_lock(data_mutex);
        for (auto &p : local_coms_size) coms_size_distr[p.first] += p.second;
        for (auto &n : local_removed) remaining_singletons.erase(n);
    }

    // stampa in un colpo solo
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << oss.str();
    }
}

int main(int argc, char* argv[]) {

    using namespace std::chrono;

    auto time_start_total = high_resolution_clock::now(); 

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

    auto time_start_serial = high_resolution_clock::now();

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

    for (auto& component : components) {
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
    for (auto it = seq_names.begin(); it != seq_names.end(); ++it) {
        remaining_singletons.insert(it->first);
    }

    std::map<size_t, node_id_t> coms_size_distr;
    std::atomic<int> nof_coms = 0;

    auto time_start_parallel = high_resolution_clock::now();

    UmbrThreadPool pool(MAX_THREADS);

//    for (auto component : connected_components(network)) {

    auto components = connected_components(network);
    for (auto &component : components){
        #if !FAST_MODE
                sort_and_print_component(component, std::cout);
        #endif
        int max_k = get_max_collision(component, network, seq_genome);
        if (max_k > 0) {
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

        } else {
            ++nof_coms;
            {
                std::lock_guard<std::mutex> data_lock(data_mutex);
                coms_size_distr[component.size()] += 1;
                for (const node_id_t& node : component)
                    remaining_singletons.erase(node);
            }
            std::stringstream oss;
            print_family(component, seq_names, oss);
            print_family_descriptions(component, seq_descr, oss);

            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << oss.str();
            }
        }
    }

    pool.wait();

    auto time_end_parallel = high_resolution_clock::now();

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        DEBUG_PRINT("\nfinito lavoro parallelo\n");
        for (const node_id_t& node : remaining_singletons)
            std::cout << "F{ " << seq_names.at(node) << " }" << std::endl;

        for (const auto& [k, v] : coms_size_distr)
            std::cout << k << " " << v << std::endl;

        std::cout << "number of communities " << nof_coms << std::endl;

        std::cout << "----------------------------------------" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }

    auto time_end_total = high_resolution_clock::now(); 

      auto serial_duration_ms   = duration_cast<milliseconds>(time_start_parallel - time_start_serial).count();
    auto parallel_duration_ms = duration_cast<milliseconds>(time_end_parallel - time_start_parallel).count();
    auto total_duration_ms    = duration_cast<milliseconds>(time_end_total - time_start_serial).count();  // solo core

    double P = (double)parallel_duration_ms / total_duration_ms;
    double serial_frac = 1.0 - P;
    double amdahl_limit = 1.0 / (serial_frac);  // N → ∞

    std::ofstream timer_file("timer_report.txt");
    timer_file << "Serial duration: " << serial_duration_ms << " ms\n";
    timer_file << "Parallel duration: " << parallel_duration_ms << " ms\n";
    timer_file << "Total duration: " << total_duration_ms << " ms\n";
    timer_file << "Serial fraction (1 - P): " << serial_frac << "\n";
    timer_file << "Amdahl theoretical speedup limit: " << amdahl_limit << "x\n";
    timer_file.close();

    DEBUG_PRINT("end of net_clu_ng");

    return 0;
}
