#include <iostream>
#include <unistd.h>

#include "lib/Homology.hh"
#include "lib/genx/GenomesContainer.hh"
#include "utils/FileLoader.hh"
#include "utils/StopWatch.hh"

using namespace homology;
using namespace genome;
using namespace utilities;




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
    FileLoader fl(inFile);
    
    fl.loadFile(gh);
    GenomesContainer::genome_ctr rowGenomes = gh.getGenomes();
    
    size_t totalGenes = 0;
    
    std::cerr<<"Total genomes: "<<rowGenomes.size();
    for(auto rowGenome = rowGenomes.begin(); rowGenome != rowGenomes.end(); ++rowGenome) {
        totalGenes += rowGenome->size();
    }
    std::cerr<<"\nTotale genes: "<<totalGenes;
    
    size_t totalGeneComparison = 0;

//    for(auto rowGenome = rowGenomes.begin(); rowGenome != rowGenomes.end(); ++rowGenome) {
//        auto other = rowGenome;
//        totalGeneComparison += (
//            (
//                other->size() * rowGenome->size() - rowGenome->size()
//            )/2
//            );
//        ++other;
//        for(other; other != rowGenomes.end(); ++other) {
//            totalGeneComparison += (
//                other->size() * rowGenome->size()
//            );
//            ++other;
//        }
//    }
//    std::cerr<<"\nTotale genes comparisons: "<<totalGeneComparison;
    std::cerr<<"\n";
    return 0;
}
