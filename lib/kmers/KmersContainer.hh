#ifndef KMER_CONTAINER_INCLUDE_GUARD
#define KMER_CONTAINER_INCLUDE_GUARD 1

#include <string>
#include <iostream>
#include <cstddef>
#include <boost/unordered_map.hpp>
#include <map>

#include "KmerMapper.hh"
#include "../VariablesTypes.hh"


/**
 * @file KmersContainer.hh
 * @brief Definitions for the KmersContainer class.
 */

/**
 * @namespace kmers
 * @brief Namespace containing definitions for kmer related classes.
 */
namespace kmers {
    
    /**
     * @class KmersContainer
     * @brief Represents a container for kmers with associated properties and operations.
     */
    class KmersContainer {

        private:
            // alias
            using sequence_t = shared::sequenceType;
            using sequence_tr = sequence_t&;
            using multipicity_t = shared::multiplicityType;
            using index_t = shared::indexType;
            using k_t = shared::kType;
            using mapKey_t = index_t;
            
            using k_dictionary_tmp = std::map<mapKey_t, multipicity_t>;
            using k_dictionary_t = std::vector<std::pair<mapKey_t, multipicity_t>>;
            using kmerSet_t = k_dictionary_t;

            k_t k_;
            sequence_t alphabet_;
            std::size_t alphabetLength_;
            multipicity_t multiplicityNumber_;
            
            index_t kmersNumber_;
            
            // Set of kmers
            k_dictionary_t dictionary_;
            mapKey_t smallerKey_;
            mapKey_t biggerKey_;
            multipicity_t smallerMultip_;
            multipicity_t biggerMultip_;

        public:
            using kmerSet_tr = const kmerSet_t&;

            /**
             * @brief Deleted default constructor.
             */
            KmersContainer() = delete;
            
            /**
             * @brief Constructs a KmersContainer object with specified parameters.
             * 
             * @param k_length Length of the kmers.
             * @param alphabet Alphabet for generating kmers.
             * @param alphabetLength Length of the alphabet.
             */
            inline explicit KmersContainer(k_t k_length, const sequence_tr alphabet, std::size_t alphabetLength) noexcept;
            
            /**
             * @brief Copy constructor.
             * 
             * @param other KmersContainer object to copy.
             */
            inline explicit KmersContainer(const KmersContainer& other) noexcept;
            
            /**
             * @brief Copy assignment operator.
             * 
             * @param other KmersContainer object to assign.
             * @return Reference to the assigned KmersContainer object.
             */
            inline KmersContainer& operator=(const KmersContainer& other) noexcept;

            /**
             * @brief Move constructor.
             * 
             * @param other KmersContainer object to move.
             */
            inline explicit KmersContainer(KmersContainer&& other) noexcept;

            /**
             * @brief Move assignment operator.
             * 
             * @param other KmersContainer object to move.
             * @return Reference to the moved KmersContainer object.
             */
            inline KmersContainer& operator=(KmersContainer&& other) noexcept;

            /**
             * @brief Default destructor.
             */
            inline ~KmersContainer() = default;


            /**
             * @brief Retrieves the set of kmers stored in the container.
             * 
             * @return Constant reference to the set of kmers.
             */
            inline kmerSet_tr getKmerSet() const;

            /**
             * @brief Retrieves the length of the kmers.
             * 
             * @return Length of the kmers.
             */
            inline k_t getK() const noexcept;

            /**
             * @brief Retrieves the length of the alphabet.
             * 
             * @return Length of the alphabet.
             */
            inline std::size_t getAlphabetLength() const noexcept;    

            /**
             * @brief Retrieves the total number of different kmers stored in the container.
             * 
             * @return Total number of kmers.
             */        
            inline index_t getDifferentKmersNumber() const noexcept;

            /**
             * @brief Retrieves the number of possible kmers based on the alphabet and kmer length.
             * 
             * @return Number of possible kmers.
             */
            inline index_t getMultiplicityNumber() const noexcept;


            /**
             * @brief Calculates kmers for the container using a provided kmer mapper.
             * 
             * @param mapper Reference to a KmerMapper object.
             */
            inline void calculateKmers(KmerMapper& mapper);

            /**
             * @brief Retrieves the alphabet used for generating kmers.
             * 
             * @return Alphabet used for generating kmers.
             */
            inline sequence_t getAlphabet() const;

            /**
             * @brief Retrieves the smallest key in the kmer dictionary.
             * 
             * @return Smallest key in the dictionary.
             */
            inline mapKey_t getSmallerKey() const;
            
            /**
             * @brief Retrieves the largest key in the kmer dictionary.
             * 
             * @return Largest key in the dictionary.
             */
            inline mapKey_t getBiggerKey() const;
            
            /**
             * @brief Retrieves the multiplicity of the smallest key in the kmer dictionary.
             * 
             * @return Multiplicity of the smallest key in the dictionary.
             */
            inline multipicity_t getSmallerKeyMultiplicity() const;


            /**
             * @brief Retrieves the multiplicity of the largest key in the kmer dictionary.
             * 
             * @return Multiplicity of the largest key in the dictionary.
             */
            inline multipicity_t getBiggerKeyMultiplicity() const;


            /**
             * @brief Prints the dictionary containing kmers and their multiplicities.
             * 
             * @param os Reference to the output stream where the dictionary will be printed.
             */
            inline void printDictionary(std::ostream& os) const;
    };
    

    // alphabet length passed is not compared with the real length of the alphabet
    inline
    KmersContainer::KmersContainer(k_t k_length, const sequence_tr alphabet, std::size_t alphabetLength) noexcept
        : k_(k_length), alphabet_(alphabet), alphabetLength_(alphabetLength), multiplicityNumber_(alphabetLength - k_length + 1), kmersNumber_(0),
        smallerKey_(0), biggerKey_(0), smallerMultip_(0), biggerMultip_(0)  {
    }

    inline
    KmersContainer::KmersContainer(const KmersContainer &other) noexcept
    : k_(other.k_), alphabet_(other.alphabet_), alphabetLength_(other.alphabetLength_), multiplicityNumber_(other.multiplicityNumber_), kmersNumber_(other.kmersNumber_),
    dictionary_(other.dictionary_), smallerKey_(other.smallerKey_), biggerKey_(other.biggerKey_) , smallerMultip_(other.smallerMultip_), biggerMultip_(other.biggerMultip_) {
    }

    inline KmersContainer
    &KmersContainer::operator=(const KmersContainer &other) noexcept {
        if(this != &other) {
            k_ = other.k_;
            alphabet_ = other.alphabet_;
            alphabetLength_ = other.alphabetLength_;
            multiplicityNumber_ = other.multiplicityNumber_;
            kmersNumber_ = other.kmersNumber_;
            dictionary_ = other.dictionary_;
            smallerKey_ = other.smallerKey_;
            biggerKey_ = other.biggerKey_;
            smallerMultip_ = other.smallerMultip_;
            biggerMultip_ = other.biggerMultip_;
        }
        return *this;
    }

    inline
    KmersContainer::KmersContainer(KmersContainer &&other) noexcept
    : k_(other.k_), alphabet_(std::move(other.alphabet_)), alphabetLength_(other.alphabetLength_), multiplicityNumber_(other.multiplicityNumber_), kmersNumber_(other.kmersNumber_),
    dictionary_(std::move(other.dictionary_)), smallerKey_(other.smallerKey_), biggerKey_(other.biggerKey_), smallerMultip_(other.smallerMultip_), biggerMultip_(other.biggerMultip_)  {
    }

    inline KmersContainer&
    KmersContainer::operator=(KmersContainer &&other) noexcept {
        if(this != &other) {
            k_ = other.k_;
            alphabet_ = std::move(other.alphabet_);
            alphabetLength_ = other.alphabetLength_;
            multiplicityNumber_ = other.multiplicityNumber_;
            dictionary_ = std::move(other.dictionary_);
            smallerKey_ = other.smallerKey_;
            kmersNumber_ = other.kmersNumber_;
            biggerKey_ = other.biggerKey_;
            smallerMultip_ = other.smallerMultip_;
            biggerMultip_ = other.biggerMultip_;
        }
        return *this;
    }

    inline KmersContainer::k_t
    KmersContainer::getK() const noexcept {
        return k_;
    }
    
    inline
    std::size_t KmersContainer::getAlphabetLength() const noexcept {
        return alphabetLength_;
    }

    //! preclude che non ci siano kmers
    inline void
    KmersContainer::calculateKmers(KmerMapper& mapper) {
        
        k_dictionary_tmp tmpDic;
        
        {
            sequence_t ss = alphabet_.substr(0, k_);

            index_t index = mapper.mapAndGetIndex(ss);
            auto pair = std::make_pair(index, 1);
            
            tmpDic.insert(std::move(pair));
            smallerKey_ = index;
            biggerKey_ = index;
            ++kmersNumber_;
        }
        
        for(index_t i = 1; i < multiplicityNumber_; ++i){
            sequence_t ss = alphabet_.substr(i, k_);

            #ifdef DEBUG
                std::cerr<<"\ni: "<<i<<"\nss: "<<ss<<"\n";
            #endif
            index_t index = mapper.mapAndGetIndex(ss);
            auto iter = tmpDic.find(index);

            if(iter == tmpDic.end()) {
                auto pair = std::make_pair(index, 1);
                tmpDic.insert(std::move(pair));
                
                if(index < smallerKey_)
                    smallerKey_ = index;
                else if(index > biggerKey_)
                    biggerKey_ = index;
                
                ++kmersNumber_;
            } else {
                ++(iter->second);
            }
        }
        smallerMultip_ = tmpDic.at(smallerKey_);
        biggerMultip_ = tmpDic.at(biggerKey_);

        for(auto i = tmpDic.begin(); i != tmpDic.end(); ++i)
            dictionary_.push_back(std::make_pair(i->first, i->second));

    }
    

    // inline void
    // KmersContainer::deleteDictionary(){
    //     dictionary_.clear();
    //     kmersNumber_ = 0;
    // }


    inline KmersContainer::kmerSet_tr
    KmersContainer::getKmerSet() const {
        return dictionary_;
    }
    

    inline KmersContainer::index_t
    KmersContainer::getDifferentKmersNumber() const noexcept {
        return kmersNumber_;
    }

    inline KmersContainer::index_t
    KmersContainer::getMultiplicityNumber() const noexcept{
        return multiplicityNumber_;
    }
    inline KmersContainer::sequence_t
    KmersContainer::getAlphabet() const {
        return alphabet_;
    }
    inline KmersContainer::mapKey_t
    KmersContainer::getSmallerKey() const {
        return smallerKey_;
    }
    inline KmersContainer::mapKey_t
    KmersContainer::getBiggerKey() const {
        return biggerKey_;
    }

    inline KmersContainer::multipicity_t
    KmersContainer::getSmallerKeyMultiplicity() const {
        return smallerMultip_;
    }
    inline KmersContainer::multipicity_t
    KmersContainer::getBiggerKeyMultiplicity() const {
        return biggerMultip_;
    }

    inline void
    KmersContainer::printDictionary(std::ostream& os) const {
        std::cerr<<"\naphabet: "<<alphabet_;

        for(auto it = dictionary_.begin(); it != dictionary_.end(); ++it){
            os<<"\n";
            // os<<it->first<<") {"<<alphabet_.substr(it->second.getFirstIndex(), k_)<<"}:\n";
            // it->second.print(os);
        }
    }

}


#endif