#include <string>
#include <iostream>
#include <cstddef>
#include <vector>
#include <memory>
#include "./Gene.hh"

#ifndef GENOME_INCLUDE_GUARD
#define GENOME_INCLUDE_GUARD

// #define DEBUG 1
namespace genome {
    
    class Genome {
        private:
            using indexType = std::size_t;
            using geneType = gene::Gene;
            using genePointer = geneType*;
            using geneContainerType = std::vector<genePointer>;
            using geneContainerTypePointer = geneContainerType*;

            // ottimizzabile sostituendo gli indexType con puntatori ad interi allocati dinamicamente?
            // alloco a inizio programma una sequenza di interi e le varie classi ci fanno riferimento
            indexType genomeId_;

            //! allocazione e deallocazione gestite qui
            geneContainerTypePointer genes_;

        public:
            Genome() = delete;
            Genome(indexType genomeId);
            void addGene(indexType id, std::string alphabet);
            void print(std::ostream& os) const;
            ~Genome();
    };

    Genome::Genome(indexType genomeId) : genomeId_{genomeId} {
        genes_ = new geneContainerType();
        #ifdef DEBUG
            std::cerr<<"\nNew genome created:\n";
            std::cerr<<"- genomeId: "<<genomeId_;
        #endif
    };
    void Genome::print(std::ostream& os) const {
        os<<"GenomeId: "<<genomeId_;
        for(auto gene = genes_->begin(); gene != genes_->end(); ++gene){
            os<<"\n\t";
            (*gene)->print(os);
        }
    }
    void Genome::addGene(indexType id, std::string alphabet){
        genes_->push_back(new gene::Gene(id, alphabet));
    }

    Genome::~Genome() {
        for(auto gene = genes_->begin(); gene != genes_->end(); ++gene){
            delete (*gene);
        }
        delete genes_;
        #ifdef DEBUG
            std::cerr<<"\nGenome destroyed\n";
        #endif
    }
}


#endif