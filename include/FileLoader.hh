#include "Genome.hh"
#include <fstream>
#include <stdexcept>


#ifndef FILE_LOADER_INCLUDE_GUARD
#define FILE_LOADER_INCLUDE_GUARD
#define DEBUG 1

class FileLoader {
    private:
        using genomeType = genome::Genome;
        using genomeTypePointer = genomeType*;
        using genomeListType = std::vector<genomeTypePointer>;
        using genomeListTypePointer = genomeListType*;
        
        genomeListTypePointer genomeList_;
        std::string fileName_;
    public:
        FileLoader(genomeListTypePointer genomeList, std::string fileName);
        void loadFile();
        ~FileLoader();
};

FileLoader::FileLoader(genomeListTypePointer genomeList, std::string fileName) : fileName_{fileName} {
    if(genomeList == nullptr)
        throw std::runtime_error("genomeList == nullptr");
    
    this->genomeList_ = genomeList;
}
void FileLoader::loadFile() {
    // open

    std::fstream file;

    // apertura in sola lettura
    file.open(fileName_, std::ios::in);
    
    if(!file.is_open())
        exit(-1);
    
    // loading
    std::string prevGenome = "";
    std::string prevGene = "";

    std::string lastGenome = "";
    std::string lastGene = "";

    std::string lastLine;

    unsigned long genomeId = -1;
    unsigned long geneId = -1;

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
                genomeList_->push_back(new genome::Genome(genomeId));
            }else{
                ++geneId;
            }
            prevGenome = lastGenome;
        }else{
            if(lastGene != prevGene){
                genomeList_->at(genomeId)->addGene(geneId, lastLine);
                prevGene = lastGene;
            }
        }

        info = !info;
    }
    
    // close
    file.close();
}

FileLoader::~FileLoader() {
}

#endif