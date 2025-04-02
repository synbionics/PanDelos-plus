#ifndef GRAPH_H
#define GRAPH_H

/*
Struttura dati grafo non orientato basato su lista di adiacenza
*/

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

class Graph {
public:
    
    void addNode(int u);

    void addEdge(int u, int v, double w);

    void printGraph() const;

    bool find_node(int u) const;

    int get_number_of_nodes() const;

    int get_number_of_edges() const;

    const std::vector<std::pair<int, double>>& get_neighbors(int u) const;

private:
    std::unordered_set<int> nodes;
    std::unordered_map<int, std::vector<std::pair<int, double>>> adj;
    int number_of_edges = 0;
};

#endif
