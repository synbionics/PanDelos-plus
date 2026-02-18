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
#include <chrono>
#include "../lib/Graph.hh"
#include "../lib/kahan_v5_umb_algo.hh"
#include "../lib/types.hh"
#include "../lib/UmbrThreadPool.hh"
#include <cmath>

std::mutex data_mutex;              // per coms_size_distr e remaining_singletons

#ifndef NDEBUG
    #define DEBUG_PRINT(x) std::cout << "[DEBUG] " << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

struct ComponentResult {
    int nof = 0;
    std::unordered_map<size_t, node_id_t> sizes;
    std::vector<node_id_t> erased;
    std::string output;
};


ComponentResult process_component(
    const std::vector<node_id_t>& component,
    const Graph& network,
    const std::unordered_map<int, std::string>& seq_genome,
    const std::unordered_map<int, std::string>& seq_names,
    const std::unordered_map<int, std::string>& seq_descr,
    UmbrThreadPool& pool,
    node_id_t THRESHOLD
) {
    ComponentResult result;
    std::stringstream oss;

    SubGraph component_subnet = SubGraph(network,component);

    auto communities = split_until_max_k(component, component_subnet, seq_genome, pool, THRESHOLD);
    result.nof = static_cast<int>(communities.size());

    for (auto& community : communities) {
        result.sizes[community.size()]++;

        for (const node_id_t& node : community)
            result.erased.push_back(node);

        print_family(community, seq_names, oss);
        print_family_descriptions(community, seq_descr, oss);
    }

    result.output = oss.str();
    return result;
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

    DEBUG_PRINT("Checking duplicates");
    check_duplicates(seq_names);

    Graph network = build_graph_from_file(net_ifile_name);

    DEBUG_PRINT("number of network nodes: " << network.get_number_of_nodes());
    DEBUG_PRINT("number of network edges: " << network.get_number_of_edges());

    std::unordered_map<size_t, node_id_t> comps_size_distr;
    int nof_comps = 0;
    DEBUG_PRINT("----------------------------------------");
    DEBUG_PRINT("Computing connected components...");
    auto components = connected_components(network);

    std::vector<size_t> sizes;
    sizes.reserve(components.size());

    for (const auto& component : components) {
        size_t comp_size = component.size();
        ++(comps_size_distr[comp_size]);
        ++nof_comps;

        sizes.push_back(comp_size);
    }

    if(sizes.empty())
        return 0;

    size_t MAX_THREADS;
    
    if (argc >= 4) {
        MAX_THREADS = std::stoi(argv[3]);
    } else {
        MAX_THREADS = std::thread::hardware_concurrency();
    }


    //calcolo THRESHOLD
    node_id_t THRESHOLD;
    size_t n = sizes.size();
    size_t idx;

    double percentile = (nof_comps < MAX_THREADS) ? 0.3 : 0.99;
    size_t pos = std::min(n - 1, static_cast<size_t>(percentile * n));
    auto it = sizes.begin() + pos;

    std::nth_element(sizes.begin(), it, sizes.end());
    THRESHOLD = *it;

    for (const auto& [size, count] : comps_size_distr)
        DEBUG_PRINT("con dimensione: " << size << " ci sono: " << count << " componenti");

    DEBUG_PRINT("number of connected components: " << nof_comps);
    DEBUG_PRINT("----------------------------------------");

    std::unordered_set<int> remaining_singletons;
    for (auto it = seq_names.begin(); it != seq_names.end(); ++it) {
        remaining_singletons.insert(it->first);
    }

    std::unordered_map<size_t, node_id_t> coms_size_distr;
    std::atomic<int> nof_coms = 0;

    UmbrThreadPool pool(MAX_THREADS);

    UmbrThreadPool bw_pool(MAX_THREADS);

    std::vector<ComponentResult> results;
    std::mutex results_mutex;

    for (auto& component : components)
        sort_and_print_component(component, std::cout);

    auto time_start_parallel = high_resolution_clock::now();

    // criterio per determinare su quante componenti ciascun thread lavora, utile per regolare la granularità
    size_t BATCH = 3; //std::max<size_t>(1, components.size() / (MAX_THREADS * 4));

    for (size_t i = 0; i < components.size(); i += BATCH) {

        pool.execute([&, i] {

            size_t end = std::min(i + BATCH, components.size());

            std::vector<ComponentResult> local_results;
            std::stringstream local_output;

            for (size_t j = i; j < end; ++j) {

                auto& component = components[j];

                int max_k = get_max_collision(component, network, seq_genome);

                if (max_k > 0) {

                    ComponentResult r = process_component(
                        component,
                        network,
                        seq_genome,
                        seq_names,
                        seq_descr,
                        bw_pool,
                        THRESHOLD
                    );

                local_results.push_back(std::move(r));

            } else {

                ++nof_coms;

                {
                    std::lock_guard<std::mutex> data_lock(data_mutex);

                    coms_size_distr[component.size()] += 1;

                    for (const node_id_t& node : component)
                        remaining_singletons.erase(node);
                }

                print_family(component, seq_names, local_output);
                print_family_descriptions(component, seq_descr, local_output);
            }
        }

        {
            std::lock_guard<std::mutex> lock(results_mutex);

            for (auto& r : local_results)
                results.push_back(std::move(r));

            std::cout << local_output.str();
        }

    });
}

    pool.wait();

    auto time_end_parallel = high_resolution_clock::now();

    for (auto& r : results) {
        nof_coms += r.nof;

        for (auto& [k, v] : r.sizes)
            coms_size_distr[k] += v;

        for (auto n : r.erased)
            remaining_singletons.erase(n);

        std::cout << r.output;
    }

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

    int target_n = 10; // il mio n.di core fisici
    double speedup_at_n = 1.0 / (serial_frac + (P / target_n));

    std::ofstream timer_file("timer_report.txt");
    timer_file << "Serial duration: " << serial_duration_ms << " ms\n";
    timer_file << "Parallel duration: " << parallel_duration_ms << " ms\n";
    timer_file << "Total duration: " << total_duration_ms << " ms\n";
    timer_file << "Serial fraction (1 - P): " << serial_frac << "\n";
    timer_file << "Amdahl theoretical speedup limit (N -> inf): " << amdahl_limit << "x\n";
    timer_file << "Amdahl speedup estimate with " << target_n << " cores: " << speedup_at_n << "x\n";
    timer_file.close();

    DEBUG_PRINT("end of net_clu_ng");

    return 0;
}
