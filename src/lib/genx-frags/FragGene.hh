#ifndef FragFRAG_GENE_INCLUDE_GUARD
#define FragFRAG_GENE_INCLUDE_GUARD 1

#include <cstddef>
#include <string>
#include <stdexcept>

#include "../VariablesTypes.hh"
#include "../kmers/KmersContainer.hh"
#include "../kmers/KmerMapper.hh"
#include "../ScoresContainer.hh"



/**
 * @file FragGene.hh
 * @brief Definitions for the FragGene class.
 */

/**
 * @namespace gene
 * @brief Namespace containing definitions for score related classes.
 */

namespace gene {


    /**
     * @class FragGene
     * @brief Represents a Fraggene in the Fraggenetic sequence.
     *
     * This class encapsulates the properties and behavior of a Fraggene, including its ID,
     * alphabet (sequence of characters), genome ID, file position, and kmers.
     */
    class FragGene {
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
            index_t fragValue_;
            sequence_t alphabet_;
            kmersContainer_tp kmers_;
            alphabetSize_t alphabetLength_;
            alphabetSize_t alphabetCutted_;
            index_t geneFilePosition_;
            std::size_t kmersNumber_;
            
        public:

            inline FragGene() = delete;
            /**
             * @brief Constructs a new FragGene object with the specified attributes.
             * @param geneId The unique identifier of the Fraggene.
             * @param alphabet The sequence of characters representing the Fraggene.
             * @param genomeId The ID of the genome to which the Fraggene belongs.
             * @param geneFilePosition The position of the Fraggene in the file.
             */
            inline explicit FragGene(const index_t geneId, const sequence_t alphabet, const index_t genomeId, const index_t fragValue, const index_t geneFilePosition) noexcept;
            
            /**
             * @brief Copy constructor.
             * @param other The FragGene object to copy.
             */
            inline FragGene(const FragGene& other) noexcept;
            
            /**
             * @brief Copy assignment operator.
             * @param other The FragGene object to copy.
             * @return A reference to the copied FragGene object.
             */
            inline FragGene& operator=(const FragGene& other) noexcept;
            
            /**
             * @brief Move constructor.
             * @param other The FragGene object to move.
             */
            inline FragGene(FragGene&& other) noexcept;
            
            /**
             * @brief Move assignment operator.
             * @param other The FragGene object to move.
             * @return A reference to the moved FragGene object.
             */
            inline FragGene& operator=(FragGene&& other) noexcept;

            
            
            /**
             * @brief Creates new kmersContainer for the Fraggene.
             * The existing KmersContainer associated with the Fraggene must be deallocated prior to calling this method.
             * @param k The length of the kmers.
             */
            inline void createNewKmers(const k_t k); // elimina i kmers precedenti

            /**
             * @brief Calculates the kmers for the Fraggene using the specified KmerMapper.
             * The KmersContainer must have been created previously.
             * 
             * @param mapper The KmerMapper to use for calculation.
             */
            inline void calculateKmers(kmerMapper_tr mapper);
            
            /**
             * @brief Deletes the kmersContainer associated with the Fraggene.
             * The kmersContainer must have been created previously.
             */
            inline void deleteKmers();

            /**
             * @brief Gets the alphabet of the Fraggene.
             * @return The alphabet of the Fraggene.
             */
            inline sequence_t getAlphabet() const noexcept;

            /**
             * @brief Gets the length of the Fraggene's alphabet.
             * @return The length of the Fraggene's alphabet.
             */
            inline index_t getAlphabetLength() const noexcept;
/**
             * @brief Gets the unique identifier of the Fraggene.
             * @return The unique identifier of the Fraggene.
             */
            inline index_t getId() const noexcept;

            /**
             * @brief Gets the position of the Fraggene in the file.
             * @return The position of the Fraggene in the file.
             */
            inline index_t getGeneFilePosition() const noexcept;

            /**
             * @brief Gets the ID of the genome to which the Fraggene belongs.
             * @return The ID of the genome to which the Fraggene belongs.
             */
            inline index_t getGenomeId() const noexcept;

            /**
             * @brief Prints the Fraggene information to the specified output stream.
             * @param os The output stream to which the Fraggene information will be printed.
             */
            inline void print(std::ostream& os) const;

            /**
             * @brief Gets a pointer to the container of kmers associated with the Fraggene.
             * @return A constant pointer to the container of kmers associated with the Fraggene.
             */
            inline kmersContainer_ctp getKmerContainer() const;

            /**
             * @brief Checks if two Fraggenes are equal based on their file positions.
             * @param Fraggene The Fraggene to compare with.
             * @return True if the Fraggenes are equal, false otherwise.
             */
            inline bool equals(const FragGene& Fraggene) const noexcept;

            /**
             * @brief Gets the length of the Fraggene's alphabet after cutting.
             * @return The length of the Fraggene's alphabet after cutting.
             */
            inline alphabetSize_t getCut() const;

            /**
             * @brief Gets the number of distinct kmers associated with the Fraggene.
             * @return The number of distinct kmers associated with the Fraggene.
             */
            inline size_t getKmersNum() const;
            
            inline index_t getFragValue() const;

            /**
             * @brief Destroys the FragGene object and releases associated resources.
             */
            inline ~FragGene();
    };
    
    inline
    FragGene::FragGene(const index_t geneId, const sequence_t alphabet, const index_t genomeId, const index_t fragValue, index_t geneFilePosition) noexcept
    : genomeId_(genomeId), geneId_(geneId), fragValue_(fragValue), alphabet_(alphabet), kmers_(nullptr),
    alphabetLength_(alphabet.length()), alphabetCutted_(floor(alphabet.length() * shared::cut)),
    geneFilePosition_(geneFilePosition), kmersNumber_(0) {
    }

    inline
    FragGene::FragGene(const FragGene &other) noexcept
    : genomeId_(other.genomeId_), geneId_(other.geneId_), fragValue_(other.fragValue_), alphabet_(other.alphabet_),
    kmers_(nullptr), alphabetLength_(other.alphabetLength_), alphabetCutted_(other.alphabetCutted_),
    geneFilePosition_(other.geneFilePosition_), kmersNumber_(other.kmersNumber_){
        if(other.kmers_ != nullptr) {
            kmers_ = new kmersContainer_t(*other.kmers_);
            if (kmers_ == nullptr)
                throw std::runtime_error("kmers == null");
        }
    }

    inline FragGene&
    FragGene::operator=(const FragGene &other) noexcept {
        if (this != &other) {
            genomeId_ = other.genomeId_;
            geneId_ = other.geneId_;
            fragValue_ = other.fragValue_;
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
    FragGene::FragGene(FragGene &&other) noexcept
    : genomeId_(other.genomeId_), geneId_(other.geneId_), fragValue_(other.fragValue_), alphabet_(std::move(other.alphabet_)),
    kmers_(other.kmers_), alphabetLength_(other.alphabetLength_),
    alphabetCutted_(other.alphabetCutted_), geneFilePosition_(other.geneFilePosition_), kmersNumber_(other.kmersNumber_) {
        other.kmers_ = nullptr;
    }

    inline FragGene&
    FragGene::operator=(FragGene &&other) noexcept {
        if (this != &other) {
            genomeId_ = other.genomeId_;
            geneId_ = other.geneId_;
            fragValue_ = other.fragValue_;
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
    FragGene::createNewKmers(const k_t k) {
        kmers_ = new kmersContainer_t(k, alphabet_, alphabetLength_);
    }

    // ! il kmer container deve essere almeno stato creato o calcolato
    inline void
    FragGene::deleteKmers() {
        kmersNumber_ = 0;
        delete kmers_;
        kmers_ = nullptr;
    }

    inline FragGene::sequence_t 
    FragGene::getAlphabet() const noexcept {
        return alphabet_;
    }

    
    inline FragGene::kmersContainer_ctp
    FragGene::getKmerContainer() const{
        return kmers_;
    }

    // ! il kmer handler deve essere creato precedentemente
    inline void
    FragGene::calculateKmers(kmerMapper_tr mapper) {
        kmers_->calculateKmers(mapper);
        kmersNumber_ = kmers_->getDifferentKmersNumber();
    }
    
    inline FragGene::index_t
    FragGene::getId() const noexcept {
        return geneId_;
    }

    inline FragGene::index_t
    FragGene::getGeneFilePosition() const noexcept {
        return geneFilePosition_;
    }

    inline FragGene::index_t
    FragGene::getGenomeId() const noexcept {
        return genomeId_;
    }

    inline FragGene::index_t
    FragGene::getAlphabetLength() const noexcept {
        return alphabetLength_;
    }

    inline void
    FragGene::print(std::ostream& os) const {
        os<<"\ngenome id: "<<genomeId_;
        os<<"\ngene id: "<<geneId_;
        os<<"\nfrag value: "<<fragValue_;
        os<<"\nfile pos: "<<geneFilePosition_;
        os<<"\nalph: "<<alphabet_;
        os<<"\nalphlength: "<<alphabetLength_;
    }

    inline bool
    FragGene::equals(const FragGene& Fraggene)
    const noexcept{
        return (geneFilePosition_ == Fraggene.geneFilePosition_);
    }
    inline FragGene::alphabetSize_t
    FragGene::getCut() const {
        return alphabetCutted_;
    }
    inline size_t
    FragGene::getKmersNum() const {
        return kmersNumber_;
    }

    inline FragGene::index_t
    FragGene::getFragValue() const {
        return fragValue_;
    }

    inline FragGene::~FragGene() {
        if(kmers_ != nullptr)
            delete kmers_;
    }

}


#endif