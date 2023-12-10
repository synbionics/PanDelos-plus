#include <cstddef>
#include <string>
#include <stdexcept>

#include "./KmersHandler.hh"

#ifndef GENE_INCLUDE_GUARD
#define GENE_INCLUDE_GUARD

namespace gene {

    class Gene {
        private:
            using indexType = std::size_t;
            using sequenceType = std::string;
            using kmerHandlerType = kmers::KmersHandler;
            using kmerHandlerTypePointer = kmerHandlerType*;

            indexType geneId_;
            sequenceType alphabet_;
            
            kmerHandlerTypePointer kmers_;
            
            // se voglio tenere traccia del genoma a cui appartiene
            // aggiungo una reference
            

        public:

            Gene() = delete;
            Gene(indexType geneId, sequenceType alphabet);
            void createNewKmersIfNotExists(unsigned short k);
            void createNewKmers(unsigned short k);
            void calculateKmers();
            void print(std::ostream& os) const;
            kmerHandlerTypePointer getKmerHandler();
            ~Gene();
    };
    
    Gene::Gene(indexType geneId, sequenceType alphabet) : geneId_{geneId}, alphabet_{alphabet}{
        kmers_ = nullptr;
        #ifdef DEBUG
            std::cerr<<"\nNew gene created:\n";
            std::cerr<<"\n- geneId: "<<geneId_<<"\n- alphabet: "<<alphabet_;
        #endif
    }
    void Gene::createNewKmers(unsigned short k) {
        if(kmers_ != nullptr)
            delete kmers_;
        kmers_ = new kmerHandlerType(k, alphabet_);
    }
    void Gene::createNewKmersIfNotExists(unsigned short k) {

        if(kmers_ == nullptr)
            kmers_ = new kmerHandlerType(k, alphabet_);
    }
    
    Gene::kmerHandlerTypePointer Gene::getKmerHandler(){
        return kmers_;
    }
    void Gene::calculateKmers() {
        if(kmers_ == nullptr)
            throw new std::runtime_error("kmers = nullptr");
            
        kmers_->calculateKmers();
    }


    void Gene::print(std::ostream& os) const {
        os<<"["<<geneId_<<"]: "<<alphabet_;
    }
    
    Gene::~Gene() {
        if(kmers_ != nullptr)
            delete kmers_;
        #ifdef DEBUG
            std::cerr<<"\nGene destroyed\n";
        #endif
    }
}


#endif