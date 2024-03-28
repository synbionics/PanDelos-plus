#ifndef GENE_INCLUDE_GUARD
#define GENE_INCLUDE_GUARD 1

#include <cstddef>
#include <string>
#include <stdexcept>

#include "../VariablesTypes.hh"
#include "../kmers/KmersContainer.hh"
#include "../kmers/KmerMapper.hh"
#include "../ScoresContainer.hh"



/**
 * @file Gene.hh
 * @brief Definitions for the Gene class.
 */

/**
 * @namespace gene
 * @brief Namespace containing definitions for score related classes.
 */

namespace gene {


    /**
     * @class Gene
     * @brief Represents a gene in the genetic sequence.
     *
     * This class encapsulates the properties and behavior of a gene, including its ID,
     * alphabet (sequence of characters), genome ID, file position, and kmers.
     */
    class Gene {
        private:
            using index_t = shared::indexType;
            using k_t = shared::kType;
            using sequence_t = shared::sequenceType;
            using sequence_tr = sequence_t&;
            using alphabetSize_t = std::size_t;
            using multiplicity_t = shared::multiplicityType;

            
            using kmersContainer_t = kmers::KmersContainer;
            using kmersSet_tr =  kmersContainer_t::kmerSet_tr;

            using kmerMapper_t = kmers::KmerMapper;
            using kmerMapper_tp = kmerMapper_t*;
            using kmerMapper_tr = kmerMapper_t&;
            using kmersContainer_tp = kmersContainer_t*;
        public:
            using kmersContainer_ctp = kmersContainer_tp const ;

            using kmersContainer_tr = kmersContainer_t&;
        private:

            index_t genomeId_;
            index_t geneId_;
            sequence_t alphabet_;
            kmersContainer_tp kmers_;
            alphabetSize_t alphabetLength_;
            alphabetSize_t alphabetCutted_;
            index_t geneFilePosition_;
            std::size_t kmersNumber_;
            
        public:

            inline Gene() = delete;
            /**
             * @brief Constructs a new Gene object with the specified attributes.
             * @param geneId The unique identifier of the gene.
             * @param alphabet The sequence of characters representing the gene.
             * @param genomeId The ID of the genome to which the gene belongs.
             * @param geneFilePosition The position of the gene in the file.
             */
            inline explicit Gene(const index_t geneId, const sequence_t alphabet, const index_t genomeId, const index_t geneFilePosition) noexcept;
            
            /**
             * @brief Copy constructor.
             * @param other The Gene object to copy.
             */
            inline Gene(const Gene& other) noexcept;
            
            /**
             * @brief Copy assignment operator.
             * @param other The Gene object to copy.
             * @return A reference to the copied Gene object.
             */
            inline Gene& operator=(const Gene& other) noexcept;
            
            /**
             * @brief Move constructor.
             * @param other The Gene object to move.
             */
            inline Gene(Gene&& other) noexcept;
            
            /**
             * @brief Move assignment operator.
             * @param other The Gene object to move.
             * @return A reference to the moved Gene object.
             */
            inline Gene& operator=(Gene&& other) noexcept;

            
            
            /**
             * @brief Creates new kmersContainer for the gene.
             * The existing KmersContainer associated with the gene must be deallocated prior to calling this method.
             * @param k The length of the kmers.
             */
            inline void createNewKmers(const k_t k); // elimina i kmers precedenti

            /**
             * @brief Calculates the kmers for the gene using the specified KmerMapper.
             * The KmersContainer must have been created previously.
             * 
             * @param mapper The KmerMapper to use for calculation.
             */
            inline void calculateKmers(kmerMapper_tr mapper);
            
            /**
             * @brief Deletes the kmersContainer associated with the gene.
             * The kmersContainer must have been created previously.
             */
            inline void deleteKmers();

            /**
             * @brief Gets the alphabet of the gene.
             * @return The alphabet of the gene.
             */
            inline sequence_t getAlphabet() const noexcept;

            /**
             * @brief Gets the length of the gene's alphabet.
             * @return The length of the gene's alphabet.
             */
            inline index_t getAlphabetLength() const noexcept;
/**
             * @brief Gets the unique identifier of the gene.
             * @return The unique identifier of the gene.
             */
            inline index_t getId() const noexcept;

            /**
             * @brief Gets the position of the gene in the file.
             * @return The position of the gene in the file.
             */
            inline index_t getGeneFilePosition() const noexcept;

            /**
             * @brief Gets the ID of the genome to which the gene belongs.
             * @return The ID of the genome to which the gene belongs.
             */
            inline index_t getGenomeId() const noexcept;

            /**
             * @brief Prints the gene information to the specified output stream.
             * @param os The output stream to which the gene information will be printed.
             */
            inline void print(std::ostream& os) const;

            /**
             * @brief Gets a pointer to the container of kmers associated with the gene.
             * @return A constant pointer to the container of kmers associated with the gene.
             */
            inline kmersContainer_ctp getKmerContainer() const;

            /**
             * @brief Checks if two genes are equal based on their file positions.
             * @param gene The gene to compare with.
             * @return True if the genes are equal, false otherwise.
             */
            inline bool equals(const Gene& gene) const noexcept;

            /**
             * @brief Gets the length of the gene's alphabet after cutting.
             * @return The length of the gene's alphabet after cutting.
             */
            inline alphabetSize_t getCut() const;

            /**
             * @brief Gets the number of distinct kmers associated with the gene.
             * @return The number of distinct kmers associated with the gene.
             */
            inline size_t getKmersNum() const;

            /**
             * @brief Destroys the Gene object and releases associated resources.
             */
            inline ~Gene();
    };
    
    inline
    Gene::Gene(const index_t geneId, const sequence_t alphabet, const index_t genomeId, index_t geneFilePosition) noexcept
    : genomeId_(genomeId), geneId_(geneId), alphabet_(alphabet), kmers_(nullptr),
    alphabetLength_(alphabet.length()), alphabetCutted_(alphabet.length()/shared::cut),
    geneFilePosition_(geneFilePosition), kmersNumber_(0) {
    }

    inline
    Gene::Gene(const Gene &other) noexcept
    : genomeId_(other.genomeId_), geneId_(other.geneId_), alphabet_(other.alphabet_),
    kmers_(nullptr), alphabetLength_(other.alphabetLength_), alphabetCutted_(other.alphabetCutted_),
    geneFilePosition_(other.geneFilePosition_), kmersNumber_(other.kmersNumber_){
        if(other.kmers_ != nullptr) {
            kmers_ = new kmersContainer_t(*other.kmers_);
            if (kmers_ == nullptr)
                throw std::runtime_error("kmers == null");
        }
    }

    inline Gene&
    Gene::operator=(const Gene &other) noexcept {
        if (this != &other) {
            genomeId_ = other.genomeId_;
            geneId_ = other.geneId_;
            alphabet_ = other.alphabet_;
            alphabetLength_ = other.alphabetLength_;
            alphabetCutted_ = other.alphabetCutted_;
            geneFilePosition_ = other.geneFilePosition_;
            deleteKmers();
            kmersNumber_ = other.kmersNumber_;

            if(other.kmers_ != nullptr) {
                kmers_ = new kmersContainer_t(*other.kmers_);
                if(kmers_ == nullptr)
                    throw std::runtime_error("kmers == null");
                
            }
        }
        return *this;
    }

    inline
    Gene::Gene(Gene &&other) noexcept
    : genomeId_(other.genomeId_), geneId_(other.geneId_), alphabet_(std::move(other.alphabet_)),
    kmers_(other.kmers_), alphabetLength_(other.alphabetLength_),
    alphabetCutted_(other.alphabetCutted_), geneFilePosition_(other.geneFilePosition_), kmersNumber_(other.kmersNumber_) {
        other.kmers_ = nullptr;
    }

    inline Gene&
    Gene::operator=(Gene &&other) noexcept {
        if (this != &other) {
            genomeId_ = other.genomeId_;
            geneId_ = other.geneId_;
            alphabet_ = std::move(other.alphabet_);
            alphabetLength_ = other.alphabetLength_;
            alphabetCutted_ = other.alphabetCutted_;
            deleteKmers();
            kmers_ = other.kmers_;
            kmersNumber_ = other.kmersNumber_;
            other.kmers_ = nullptr;
            geneFilePosition_ = other.geneFilePosition_;
        }
        return *this;
    }

    //! il kmers devono essere precedentemente deallocato
    inline void
    Gene::createNewKmers(const k_t k) {
        kmers_ = new kmersContainer_t(k, alphabet_, alphabetLength_);
    }

    // ! il kmer container deve essere almeno stato creato o calcolato
    inline void
    Gene::deleteKmers() {
        kmersNumber_ = 0;
        delete kmers_;
        kmers_ = nullptr;
    }

    inline Gene::sequence_t 
    Gene::getAlphabet() const noexcept {
        return alphabet_;
    }

    
    inline Gene::kmersContainer_ctp
    Gene::getKmerContainer() const{
        return kmers_;
    }

    // ! il kmer handler deve essere creato precedentemente
    inline void
    Gene::calculateKmers(kmerMapper_tr mapper) {
        kmers_->calculateKmers(mapper);
        kmersNumber_ = kmers_->getDifferentKmersNumber();
    }
    
    inline Gene::index_t
    Gene::getId() const noexcept {
        return geneId_;
    }

    inline Gene::index_t
    Gene::getGeneFilePosition() const noexcept {
        return geneFilePosition_;
    }

    inline Gene::index_t
    Gene::getGenomeId() const noexcept {
        return genomeId_;
    }

    inline Gene::index_t
    Gene::getAlphabetLength() const noexcept {
        return alphabetLength_;
    }

    inline void
    Gene::print(std::ostream& os) const {
        os<<"\ngene id: "<<geneId_;
        os<<"\nfile pos: "<<geneFilePosition_;
        os<<"\nalph: "<<alphabet_;
    }

    inline bool
    Gene::equals(const Gene& gene)
    const noexcept{
        return (geneFilePosition_ == gene.geneFilePosition_);
    }
    inline Gene::alphabetSize_t
    Gene::getCut() const {
        return alphabetCutted_;
    }
    inline size_t
    Gene::getKmersNum() const {
        return kmersNumber_;
    }

    inline Gene::~Gene() {
        if(kmers_ != nullptr)
            delete kmers_;
    }

}


#endif