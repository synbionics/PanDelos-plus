#include <string>
#include <iostream>

#ifndef GENOME_INCLUDE_GUARD
#define GENOME_INCLUDE_GUARD

// #define DEBUG 1
namespace genome {
    
    //! Ulterione livello separando genoma e gene????
    //!
    class Genome {
        private:
            using indexType = std::size_t;
            using sequenceType = std::string;
            // ottimizzabile sostituendo gli indexType con puntatori ad interi allocati dinamicamente
            indexType genomeId_;
            indexType geneId_;
            sequenceType alphabet_;
            // std::string note;

        public:
            Genome() = delete;
            Genome(indexType genomeId, indexType geneId, sequenceType alphabet) : genomeId_{genomeId}, geneId_{geneId}, alphabet_{alphabet} {
                
                #ifdef DEBUG
                    std::cerr<<"\nNew genome created:\n"
                    std::cerr<<"- genomeId: "<<genomeId_<<"\n- geneId: "<<geneId_<<"\n- alphabet: "<<alphabet_;
                #endif
            };
            ~Genome();
    };

    // Genome::Genome(indexType genomeId, indexType geneId, sequenceType nucleotides){}

    Genome::~Genome() {
    }

}


#endif