#include <iostream>
#include <unistd.h>

#include "lib/Homology.hh"
#include "lib/genx/GenomesContainer.hh"
#include "utils/FileLoaderRef.hh"
#include "utils/StopWatch.hh"

using namespace homology;
using namespace genome;




int main(int argc, char *argv[]){

    int k;
    ushort threadNum;
    std::string inFile;
    std::string statFile;
    std::string outFile;
    
    int option;
    while ((option = getopt(argc, argv, "i:o:k:h")) != -1) {
        switch (option) {
            case 'i':
                inFile = optarg;
                break;
        }
    }
    
    GenomesContainer gh;
    FileLoaderRef fl(inFile);
    
    fl.loadFile(gh);
    GenomesContainer::genome_ctr rowGenomes = gh.getGenomes();
    
    size_t totalGenes = 0;
    
    
    std::cerr<<"Genome,Genes\n";
    for(auto rowGenome = rowGenomes.begin(); rowGenome != rowGenomes.end(); ++rowGenome) {
        std::cerr<<rowGenome->getId()<<","<<rowGenome->size()<<"\n";
        totalGenes += rowGenome->size();
    }
    std::cerr<<"\n";
    std::cerr<<"Total genomes: "<<rowGenomes.size();
    std::cerr<<"\nTotale genes: "<<totalGenes;
    
    size_t totalGeneComparison = 0;

    std::cerr<<"\n";
    return 0;
}
