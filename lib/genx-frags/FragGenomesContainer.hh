#ifndef FRAG_GENOME_CONTAINER_INCLUDE_GUARD
#define FRAG_GENOME_CONTAINER_INCLUDE_GUARD 1

#include <vector>

#include "FragGenome.hh"
#include "../VariablesTypes.hh"




namespace genome {

    class FragGenomesContainer {
        private:
            using genome_t = genome::FragGenome;
            using index_t = shared::indexType;
            using sequence_t = shared::sequenceType;
        public:
            using genome_tp = genome_t*;
            using genome_tr = genome_t&;
        private:
            using genome_ct = std::vector<genome_t>;
        public:
            using genome_ctp = genome_ct*;
            using genome_ctr = genome_ct&;
        
        private:
            index_t size_;
            genome_ct genomes_;

        public:
            inline explicit FragGenomesContainer() noexcept;
            FragGenomesContainer(const FragGenomesContainer&) = delete;
            FragGenomesContainer& operator=(const FragGenomesContainer&) = delete;
            FragGenomesContainer(FragGenomesContainer&&) = delete;
            FragGenomesContainer& operator=(FragGenomesContainer&&) = delete;
            
            inline ~FragGenomesContainer();
            
            inline void addGenome(const index_t id);
            inline genome_tr const getGenomeAt(index_t id);
            inline void addGeneToGenome(
                const index_t genomeId, const index_t geneId, const index_t fragValue, const sequence_t alphabet, const index_t geneFilePosition);
            inline index_t size() const noexcept;
            inline genome_ctr getGenomes();
    };

    inline
    FragGenomesContainer::FragGenomesContainer() noexcept : size_(0) {}
    
    inline
    FragGenomesContainer::~FragGenomesContainer() {}


    // aggiunge un genoma
    inline void
    FragGenomesContainer::addGenome(const index_t id) {
        // std::cerr<<"\nAdding new genome "<<id<<std::endl;
        genome_t newGenome(id);
        ++size_;
        // std::cerr<<"\nsize: "<<size_<<std::endl;
        genomes_.push_back(std::move(newGenome));
    }

    inline FragGenomesContainer::genome_tr const
    FragGenomesContainer::getGenomeAt(const index_t id) {
        if(id >= size_)
            throw std::runtime_error("id >= size");
        return genomes_[id];
    }

    inline void
    FragGenomesContainer::addGeneToGenome(const index_t genomeId, const index_t geneId, const index_t fragValue, sequence_t alphabet, const index_t geneFilePosition) {
        if(genomeId >= size_)
            throw std::runtime_error("genomeId >= size");
        genomes_[genomeId].addGene(geneId, alphabet, fragValue, geneFilePosition);
    }

    inline FragGenomesContainer::index_t
    FragGenomesContainer::size() const noexcept {
        return size_;
    }

    inline FragGenomesContainer::genome_ctr
    FragGenomesContainer::getGenomes() {
        return genomes_;
    }
}


#endif