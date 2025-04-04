#ifndef GENOME_CONTAINER_INCLUDE_GUARD
#define GENOME_CONTAINER_INCLUDE_GUARD 1

#include <vector>

#include "Genome.hh"
#include "../VariablesTypes.hh"




namespace genome {

    class GenomesContainer {
        private:
            using genome_t = genome::Genome;
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
            inline explicit GenomesContainer() noexcept;
            GenomesContainer(const GenomesContainer&) = delete;
            GenomesContainer& operator=(const GenomesContainer&) = delete;
            GenomesContainer(GenomesContainer&&) = delete;
            GenomesContainer& operator=(GenomesContainer&&) = delete;
            
            inline ~GenomesContainer();
            
            inline void addGenome(index_t id);
            inline genome_tr const getGenomeAt(index_t id);
            inline void addGeneToGenome(
                const index_t genomeId, const index_t geneId, const sequence_t alphabet, const index_t geneFilePosition);
            inline index_t size() const noexcept;
            inline genome_ctr getGenomes();
    };

    inline
    GenomesContainer::GenomesContainer() noexcept : size_(0) {}
    
    inline
    GenomesContainer::~GenomesContainer() {}


    // aggiunge un genoma
    inline void
    GenomesContainer::addGenome(const index_t id) {
        genome_t newGenome(id);
        ++size_;
        genomes_.push_back(std::move(newGenome));
    }

    inline GenomesContainer::genome_tr const
    GenomesContainer::getGenomeAt(const index_t id) {
        if(id >= size_)
            throw std::runtime_error("id >= size");
        return genomes_[id];
    }

    inline void
    GenomesContainer::addGeneToGenome(const index_t genomeId, const index_t geneId, sequence_t alphabet, const index_t geneFilePosition) {
        if(genomeId >= size_)
            throw std::runtime_error("genomeId >= size");
        genomes_[genomeId].addGene(geneId, alphabet, geneFilePosition);
    }

    inline GenomesContainer::index_t
    GenomesContainer::size() const noexcept {
        return size_;
    }

    inline GenomesContainer::genome_ctr
    GenomesContainer::getGenomes() {
        return genomes_;
    }
}


#endif