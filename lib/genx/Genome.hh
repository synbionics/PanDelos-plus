#ifndef GENOME_INCLUDE_GUARD
#define GENOME_INCLUDE_GUARD 1

#include <string>
#include <iostream>
#include <exception>
#include <cstddef>
#include <vector>
#include <memory>
#include "../VariablesTypes.hh"
#include "Gene.hh"
#include "../../threads/ThreadPool.hh"

/**
 * @file Genome.hh
 * @brief Definitions for the Genome class.
 */

/**
 * @namespace genome
 * @brief Namespace containing definitions for score related classes.
 */
namespace genome {
    
    class Genome {
        private:
            using index_t = shared::indexType;
            using gene_t = gene::Gene;
            using kmerMapper_t = kmers::KmerMapper;
            using kmerMapper_tp = kmerMapper_t*;
            using kmerMapper_tr = kmerMapper_t&;
            using sequence_t = shared::sequenceType;
            using sequence_tr = sequence_t&;
            using sequence_tp = sequence_t*;
            using k_t = shared::kType;

            using thread_pt = threads::ThreadPool;
            using thread_ptr = thread_pt&;

            
        public:
            using gene_tp = gene_t*;
            using gene_tr = gene_t&;
            
        private:
            using gene_ct = std::vector<gene_t>;
        public:
            using gene_ctp = gene_ct*;
            using gene_ctr = gene_ct&;
        private:

            index_t genomeId_;
            index_t size_;
            gene_ct genes_;

        public:

            Genome() = delete;
            /**
             * @brief Constructs a Genome object with the given genome ID.
             * @param genomeId The ID of the genome.
             */
            explicit Genome(const index_t genomeId) noexcept;

            /**
             * @brief Copy constructor for Genome objects.
             * @param other The Genome object to be copied.
             */
            explicit Genome(const Genome& other) noexcept;

            /**
             * @brief Assignment operator for Genome objects.
             * @param other The Genome object to be assigned.
             * @return A reference to the assigned Genome object.
             */
            Genome& operator=(const Genome& other) noexcept;

            /**
             * @brief Move constructor for Genome objects.
             * @param other The Genome object to be moved.
             */
            explicit Genome(Genome&& other) noexcept;

            /**
             * @brief Move assignment operator for Genome objects.
             * @param other The Genome object to be moved.
             * @return A reference to the moved Genome object.
             */
            Genome& operator=(Genome&& other) noexcept;

            inline ~Genome() = default;
            

            /**
             * @brief Adds a new gene to the genome.
             * @param id The ID of the gene.
             * @param alphabet The sequence alphabet of the gene.
             * @param geneFilePosition The file position of the gene.
             */
            inline void addGene(const index_t id, sequence_t alphabet, const index_t geneFilePosition);

            /**
             * @brief Gets a constant reference to the vector of genes in the genome.
             * @return A constant reference to the vector of genes.
             */
            inline gene_ctr const getGenes();
            
            /**
             * @brief Gets a constant reference to the gene at the specified index.
             * @param index The index of the gene to retrieve.
             * @return A constant reference to the gene at the specified index.
             */
            inline gene_tr const getGeneAt(index_t index);
            
            /**
             * @brief Creates and calculates kmers for all genes in the genome.
             * @param k The length of kmers.
             * @param mapper The k-mer mapper object to use.
             */
            inline void createAndCalculateAllKmers(k_t k, kmerMapper_tr mapper);
            
            /**
             * @brief Deletes all kmers associated with genes in the genome.
             * @param pool The thread pool object to execute deletion in parallel.
             */
            inline void deleteAllKmers(thread_ptr pool);


            /**
             * @brief Checks if two genomes are equal.
             * @param other The other genome to compare.
             * @return True if the genomes are equal, false otherwise.
             */
            bool operator==(const Genome& other) const;

            /**
             * @brief Gets the ID of the genome.
             * @return The ID of the genome.
             */
            index_t getId() const noexcept;

            /**
             * @brief Gets the number of genes in the genome.
             * @return The number of genes in the genome.
             */
            index_t size() const noexcept;

            /**
             * @brief Prints the genome information.
             * @param os The output stream to print to.
             */
            void print(std::ostream& os) const;
    };

    inline
    Genome::Genome(const index_t genomeId) noexcept
    : genomeId_(genomeId), size_(0) {
        #ifdef DEBUG
            std::cerr<<"\nNew genome created:\n";
            std::cerr<<"- genomeId: "<<genomeId_;
        #endif
    }
    
    inline
    Genome::Genome(const Genome &other) noexcept
    : genomeId_(other.genomeId_), size_(other.size_), genes_(other.genes_) {}
    
    inline Genome&
    Genome::operator=(const Genome &other) noexcept {
        if(this != &other) {
            genomeId_ = other.genomeId_;
            size_ = other.size_;
            genes_ = other.genes_;
        }
        return *this;
    }
    
    inline
    Genome::Genome(Genome &&other) noexcept
    : genomeId_(other.genomeId_), size_(other.size_), genes_(std::move(other.genes_)) {
    }

    inline Genome&
    Genome::operator=(Genome &&other) noexcept {
        if(this != &other) {
            genomeId_ = other.genomeId_;
            size_ = other.size_;
            genes_ = std::move(other.genes_);
        }
        return *this;
    };

    inline void
    Genome::print(std::ostream& os) const {
        os<<"GenomeId: "<<genomeId_;
        for(auto gene = genes_.begin(); gene != genes_.end(); ++gene){
            os<<"\n\t";
            gene->print(os);
        }
        os<<"\n\n";
    }

    inline void
    Genome::addGene(const index_t id, sequence_t alphabet, const index_t geneFilePosition) {
        genes_.push_back(std::move(gene_t(id, alphabet, genomeId_, geneFilePosition)));
        ++size_;
    }

    inline Genome::gene_ctr const
    Genome::getGenes() {
        return genes_;
    }

    inline Genome::gene_tr const
    Genome::getGeneAt(index_t index) {
        // if(index >= size_)
        //     throw std::runtime_error("index >= size");

        return genes_[index];
    }

    inline void
    Genome::createAndCalculateAllKmers(k_t k, kmerMapper_tr mapper){
        for(auto g = genes_.begin(); g != genes_.end(); ++g){
            auto& gRef = *g;
            gRef.createNewKmers(k);
            gRef.calculateKmers(mapper);
        }
    }
    inline void
    Genome::deleteAllKmers(thread_ptr pool) {
        for(auto g = genes_.begin(); g != genes_.end(); ++g){
            pool.execute(
                [g] {
                    g->deleteKmers();
                }
            );
        }
        // pool.waitTasks();
        while(!pool.tasksCompleted())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // ritorna il numero di geni
    inline Genome::index_t
    Genome::size() const noexcept {
        return size_;
    }
    
    // ritorna l'id del genoma
    inline Genome::index_t
    Genome::getId()
    const noexcept {
        return genomeId_;
    }
    
    // verifica se due genomi sono uguali
    inline bool
    Genome::operator==(const Genome& other)
    const {
        return genomeId_ == other.genomeId_;
    }
}


#endif