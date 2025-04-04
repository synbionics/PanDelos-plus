#ifndef FRAGS_FILE_LOADER_INCLUDE_GUARD
#define FRAGS_FILE_LOADER_INCLUDE_GUARD 1

#include <fstream>
#include <stdexcept>

#include "./../lib/genx-frags/FragGenomesContainer.hh"
#include "./../lib/genx-frags/FragGenome.hh"

// #define DEBUG 1

namespace utilities {
    
    class FragsFileLoader {
        private:
            
            using genome_ct = genome::FragGenomesContainer;
            using genome_ctr = genome_ct&;
            
            std::string fileName_;
        public:
            FragsFileLoader(std::string fileName);
            void loadFile(genome_ctr genomeContainer);
            ~FragsFileLoader();
    };

    FragsFileLoader::FragsFileLoader(std::string fileName) : fileName_(fileName) { }

    void FragsFileLoader::loadFile(genome_ctr genomeContainer) {
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

        unsigned long genomeId = 0;
        unsigned long geneId = 0;
        unsigned long geneLine = 0;
        unsigned long valueUL = 0;

        bool info = true;
        bool firstLine = true;
        
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

                // std::cerr<<"\nlast line: "<<lastLine;
                
                lastLine.erase(0, lastLine.find(delimiter) + delimiter.length());

                std::string product = lastLine.substr(0, lastLine.find(delimiter));
                lastLine.erase(0, lastLine.find(delimiter) + delimiter.length());

                std::string value = lastLine;
                try {
                    valueUL = std::stoul(value);
                } catch (const std::exception& e) {
                    std::cerr<<"\nError during frag input loading on genome-gene: "<<lastGenome<<"- "<<lastGene;
                    valueUL = 0;
                    
                }

                #ifdef DEBUG
                std::cerr<<"\nprev genome: "<<prevGenome;
                std::cerr<<"\nprev gene:"<<prevGene;
                std::cerr<<"\nlast genome: "<<lastGenome;
                std::cerr<<"\nlast gene: "<<lastGene;
                std::cerr<<"\nvalue: "<<valueUL;
                #endif


                if(lastGenome != prevGenome){
                    if(!firstLine){
                        ++genomeId;
                    }

                    geneId = 0;
                    prevGene = "";

                    genomeContainer.addGenome(genomeId);

                    if(firstLine){
                        firstLine = false;
                    }
                }else{
                    ++geneId;
                }
                prevGenome = lastGenome;
            }else{
                if(lastGene != prevGene){
                    genomeContainer.addGeneToGenome(genomeId, geneId, valueUL, lastLine, geneLine);
                    prevGene = lastGene;
                    ++geneLine;
                }
            }

            info = !info;
        }
        
        // close
        file.close();
    }


    FragsFileLoader::~FragsFileLoader() {
    }
}

#endif