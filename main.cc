// #include "./include/KmersHandler.hh"
#include <iostream>
#include <memory>


#include "./tests/kMersTest.hh"
#include "./tests/homologyDetectorTest.hh"
#include "./include/Kmer.hh"
#include "./include/Gene.hh"
#include "./include/Genome.hh"
#include "./include/FileLoader.hh"

int main(){
    // kmers_tests::launch();
    homology_tests::launch();
    // std::vector<genome::Genome*>* genomes = new std::vector<genome::Genome*>();
    // FileLoader fl(genomes, "NC_000913.faa");
    // FileLoader fl(genomes, "escherichia.faa");
    // fl.loadFile();
    
    // for(auto genome = genomes->begin(); genome != genomes->end(); ++genome){
    //     (*genome)->print(std::cerr);
    // }
    // std::cerr<<"\nsize: "<<genomes->size();
    // char a;
    // std::cin>>a;
    return 0;
}