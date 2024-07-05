#ifndef FragFRAG_GENOME_INCLUDE_GUARD
#define FragFRAG_GENOME_INCLUDE_GUARD 1

#include <string>
#include <iostream>
#include <exception>
#include <cstddef>
#include <vector>
#include <memory>
#include "../VariablesTypes.hh"
#include "FragGene.hh"
#include "../../threads/ThreadPool.hh"

/**
 * @file FragGenome.hh
 * @brief Definitions for the FragGenome class.
 */

/**
 * @namespace genome
 * @brief Namespace containing definitions for score related classes.
 */
namespace genome {
    
    class FragGenome {
        private:
            using index_t = shared::indexType;
            using gene_t = gene::FragGene;
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

            FragGenome() = delete;
            /**
             * @brief Constructs a FragGenome object with the given FragGenome ID.
             * @param genomeId The ID of the FragGenome.
             */
            explicit FragGenome(const index_t genomeId) noexcept;

            /**
             * @brief Copy constructor for FragGenome objects.
             * @param other The FragGenome object to be copied.
             */
            explicit FragGenome(const FragGenome& other) noexcept;

            /**
             * @brief Assignment operator for FragGenome objects.
             * @param other The FragGenome object to be assigned.
             * @return A reference to the assigned FragGenome object.
             */
            FragGenome& operator=(const FragGenome& other) noexcept;

            /**
             * @brief Move constructor for FragGenome objects.
             * @param other The FragGenome object to be moved.
             */
            explicit FragGenome(FragGenome&& other) noexcept;

            /**
             * @brief Move assignment operator for FragGenome objects.
             * @param other The FragGenome object to be moved.
             * @return A reference to the moved FragGenome object.
             */
            FragGenome& operator=(FragGenome&& other) noexcept;

            inline ~FragGenome() = default;
            

            /**
             * @brief Adds a new gene to the Fraggenome.
             * @param id The ID of the gene.
             * @param alphabet The sequence alphabet of the gene.
             * @param geneFilePosition The file position of the gene.
             */
            inline void addGene(const index_t id, sequence_t alphabet, const index_t fragVal, const index_t geneFilePosition);

            /**
             * @brief Gets a constant reference to the vector of genes in the Fraggenome.
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
             * @brief Creates and calculates kmers for all genes in the Fraggenome.
             * @param k The length of kmers.
             * @param mapper The k-mer mapper object to use.
             */
            inline void createAndCalculateAllKmers(k_t k, kmerMapper_tr mapper);
            
            /**
             * @brief Deletes all kmers associated with genes in the Fraggenome.
             * @param pool The thread pool object to execute deletion in parallel.
             */
            inline void deleteAllKmers(thread_ptr pool);


            /**
             * @brief Checks if two Fraggenomes are equal.
             * @param other The other Fraggenome to compare.
             * @return True if the Fraggenomes are equal, false otherwise.
             */
            bool operator==(const FragGenome& other) const;

            /**
             * @brief Gets the ID of the Fraggenome.
             * @return The ID of the Fraggenome.
             */
            index_t getId() const noexcept;

            /**
             * @brief Gets the number of genes in the Fraggenome.
             * @return The number of genes in the Fraggenome.
             */
            index_t size() const noexcept;

            /**
             * @brief Prints the Fraggenome information.
             * @param os The output stream to print to.
             */
            void print(std::ostream& os) const;
    };

    inline
    FragGenome::FragGenome(const index_t genomeId) noexcept
    : genomeId_(genomeId), size_(0) {
        #ifdef DEBUG
            std::cerr<<"\nNew Fraggenome created:\n";
            std::cerr<<"- genomeId: "<<genomeId_;
        #endif
    }
    
    inline
    FragGenome::FragGenome(const FragGenome &other) noexcept
    : genomeId_(other.genomeId_), size_(other.size_), genes_(other.genes_) {}
    
    inline FragGenome&
    FragGenome::operator=(const FragGenome &other) noexcept {
        if(this != &other) {
            genomeId_ = other.genomeId_;
            size_ = other.size_;
            genes_ = other.genes_;
        }
        return *this;
    }
    
    inline
    FragGenome::FragGenome(FragGenome &&other) noexcept
    : genomeId_(other.genomeId_), size_(other.size_), genes_(std::move(other.genes_)) {
    }

    inline FragGenome&
    FragGenome::operator=(FragGenome &&other) noexcept {
        if(this != &other) {
            genomeId_ = other.genomeId_;
            size_ = other.size_;
            genes_ = std::move(other.genes_);
        }
        return *this;
    };

    inline void
    FragGenome::print(std::ostream& os) const {
        os<<"GenomeId: "<<genomeId_;
        for(auto gene = genes_.begin(); gene != genes_.end(); ++gene){
            os<<"\n\t";
            gene->print(os);
        }
        os<<"\n\n";
    }

    inline void
    FragGenome::addGene(const index_t id, sequence_t alphabet, const index_t fragVal, const index_t geneFilePosition) {
        genes_.push_back(std::move(gene_t(id, alphabet, genomeId_, fragVal, geneFilePosition)));
        ++size_;
    }

    inline FragGenome::gene_ctr const
    FragGenome::getGenes() {
        return genes_;
    }

    inline FragGenome::gene_tr const
    FragGenome::getGeneAt(index_t index) {
        // if(index >= size_)
        //     throw std::runtime_error("index >= size");

        return genes_[index];
    }

    inline void
    FragGenome::createAndCalculateAllKmers(k_t k, kmerMapper_tr mapper){
        for(auto g = genes_.begin(); g != genes_.end(); ++g){
            auto& gRef = *g;
            gRef.createNewKmers(k);
            gRef.calculateKmers(mapper);
        }
    }
    inline void
    FragGenome::deleteAllKmers(thread_ptr pool) {
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
    inline FragGenome::index_t
    FragGenome::size() const noexcept {
        return size_;
    }
    
    // ritorna l'id del genoma
    inline FragGenome::index_t
    FragGenome::getId()
    const noexcept {
        return genomeId_;
    }
    
    // verifica se due genomi sono uguali
    inline bool
    FragGenome::operator==(const FragGenome& other)
    const {
        return genomeId_ == other.genomeId_;
    }
}


#endif