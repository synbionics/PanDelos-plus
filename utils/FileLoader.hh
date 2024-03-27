#ifndef FILE_LOADER_INCLUDE_GUARD
#define FILE_LOADER_INCLUDE_GUARD 1

#include <fstream>
#include <stdexcept>

#include "./../lib/genx/GenomesContainer.hh"
#include "./../lib/genx/Genome.hh"

// #define DEBUG 1

namespace utilities {
    
    class FileLoader {
        private:
            
            using genome_ct = genome::GenomesContainer;
            using genome_ctr = genome_ct&;
            
            std::string fileName_;
        public:
            FileLoader(std::string fileName);
            void loadFile(genome_ctr genomeContainer);
            ~FileLoader();
    };

    FileLoader::FileLoader(std::string fileName) : fileName_(fileName) { }

    void FileLoader::loadFile(genome_ctr genomeContainer) {
        // open

        std::fstream file;

        // apertura in sola lettura
        file.open(fileName_, std::ios::in);
        
        if(!file.is_open())
            throw std::runtime_error("missing file");
        
        // loading
        std::string prevGenome = "";
        std::string prevGene = "";

        std::string lastGenome = "";
        std::string lastGene = "";

        std::string lastLine;

        unsigned long genomeId = -1;
        unsigned long geneId = -1;
        unsigned long geneLine = 0;

        bool info = true;
        std::string delimiter = "\t";

        while(std::getline(file, lastLine)){
        
            #ifdef DEBUG
            std::cerr<<"\nlastLine: "<<lastLine;
            std::cerr<<"\ninfo: "<<info;
            #endif

            if(info){

                lastGenome = lastLine.substr(0, lastLine.find(delimiter));
                lastLine.erase(0, lastLine.find(delimiter) + delimiter.length());

                lastGene = lastLine.substr(0, lastLine.find(delimiter));

                #ifdef DEBUG
                std::cerr<<"\n"<<prevGenome;
                std::cerr<<"\n"<<prevGene;
                std::cerr<<"\n"<<lastGenome;
                std::cerr<<"\n"<<lastGene;
                #endif
                if(lastGenome != prevGenome){
                    ++genomeId;
                    geneId = 0;
                    genomeContainer.addGenome(genomeId);
                }else{
                    ++geneId;
                }
                prevGenome = lastGenome;
            }else{
                if(lastGene != prevGene){
                    genomeContainer.addGeneToGenome(genomeId, geneId, lastLine, geneLine);
                    prevGene = lastGene;
                    ++geneLine;
                }
            }

            info = !info;
        }
        
        // close
        file.close();
    }


    FileLoader::~FileLoader() {
    }
}

#endif