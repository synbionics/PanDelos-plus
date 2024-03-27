#ifndef BBH_CANDIDATES_INCLUDE_GUARD
#define BBH_CANDIDATES_INCLUDE_GUARD 1

#include "../VariablesTypes.hh"
#include <utility>
#include <iostream>
#include <boost/unordered_set.hpp>
// #define debug 1


/**
 * @file BBHCandidate.hh
 * @brief Definitions for the BBHCandidate class.
 */

/**
 * @namespace bbh
 * @brief Namespace containing definitions for Best Bidirectional Hits (BBH) related classes.
 */

namespace bbh {
    
    /**
     * @class BBHCandidate
     * @brief Represents a set of candidates for Best Bidirectional Hits (BBH).
     */
    class BBHCandidate {
        private:
            using index_t = shared::indexType;
            using score_t = shared::scoreType;

            using container_t = boost::unordered_set<index_t>;

            // numero di candidati
            index_t size_;

            // score attualmente migliore
            score_t currentBestScore_;
            
            // set di identificativi dei geni sulle colonne
            container_t candidates_;

        public:
            using container_tr = container_t&;
            using container_tcr = const container_t&;
            
            /**
             * @brief Constructs a BBHCandidate object with specified capacity.
             * 
             * @param capacity Capacity of the candidate set.
             */
            inline explicit BBHCandidate(const index_t capacity);


            /**
             * @brief Default constructor.
             */
            BBHCandidate() : size_(0), currentBestScore_(0) {}
            
            /**
             * @brief Copy constructor.
             * 
             * @param other BBHCandidate object to copy.
             */
            BBHCandidate(const BBHCandidate& other) noexcept;

            
            /**
             * @brief Move constructor.
             * 
             * @param other BBHCandidate object to move.
             */
            BBHCandidate(BBHCandidate&& other) noexcept;

            
            /**
             * @brief Copy assignment operator.
             * 
             * @param other BBHCandidate object to assign.
             * @return Reference to the assigned BBHCandidate object.
             */
            BBHCandidate& operator=(const BBHCandidate& other) noexcept;
            

            /**
             * @brief Move assignment operator.
             * 
             * @param other BBHCandidate object to move.
             * @return Reference to the moved BBHCandidate object.
             */
            BBHCandidate& operator=(BBHCandidate&& other) noexcept;

            /**
             * @brief Adds a candidate to the set with the given score and index.
             * 
             * @param score Score of the candidate.
             * @param index Index of the candidate.
             */
            inline void addCandidate(const score_t score, const index_t index);
            
            /**
             * @brief Retrieves the number of candidates in the set.
             * 
             * @return Number of candidates in the set.
             */
            inline index_t size() const noexcept;

            /**
             * @brief Retrieves a reference to the set of candidate identifiers.
             * 
             * @return Reference to the set of candidate identifiers.
             */
            inline container_tr const getCandidateList();
            /**
             * @brief Retrieves a constant reference to the set of candidate identifiers.
             * 
             * @return Constant reference to the set of candidate identifiers.
             */
            inline container_tcr getCandidateList() const;

            /**
             * @brief Retrieves the best score among the candidates.
             * 
             * @return Best score among the candidates.
             */
            inline score_t getBestScore() const noexcept;

            /**
             * @brief Default destructor.
             */
            ~BBHCandidate() = default;
            
            /**
             * @brief Prints the set of candidates to the output stream.
             * 
             * @param os Reference to the output stream.
             */
            inline void print(std::ostream& os) const;
    };

    inline
    BBHCandidate::BBHCandidate(const index_t capacity) : size_(0), currentBestScore_(0) {
        candidates_.reserve(capacity);
    }
    inline
    BBHCandidate::BBHCandidate(const BBHCandidate& other) noexcept
    : size_(other.size_), currentBestScore_(other.currentBestScore_), candidates_(other.candidates_) { }
    
    inline
    BBHCandidate::BBHCandidate(BBHCandidate&& other) noexcept
    : size_(other.size_), currentBestScore_(other.currentBestScore_), candidates_(std::move(other.candidates_)) {};

    inline
    BBHCandidate& BBHCandidate::operator=(const BBHCandidate& other) noexcept {
        if(this != &other) {
            size_ = other.size_;
            currentBestScore_ = other.currentBestScore_;
            candidates_ = other.candidates_;
        }
        return *this;
    }

    inline
    BBHCandidate& BBHCandidate::operator=(BBHCandidate&& other) noexcept {
        if(this != &other) {
            size_ = other.size_;
            currentBestScore_ = other.currentBestScore_;
            candidates_ = std::move(other.candidates_);
        }
        return *this;
    }
    
    inline void
    BBHCandidate::addCandidate(const score_t score, const index_t index) {
        
        // confronto punteggio nuovo con quello attuale:
        // se migliore resetto la lista di candidati e aggiungo il nuovo indice
        // se uguale aggiungo il nuovo indice al set
        // se minore viene ignorato

        if(score > currentBestScore_ ) {
            candidates_.clear();
            candidates_.emplace(index);
            currentBestScore_ = score;
            size_ = 1;
        } else if (score == currentBestScore_) {
            candidates_.emplace(index);
            ++size_;
        }
    }

    inline void
    BBHCandidate::print(std::ostream& os) const {
        os<<"\nscore: "<<currentBestScore_<<"\ncandidates {\n";
        for(auto it = candidates_.begin(); it != candidates_.end(); ++it)
            os<<(*it)<<" ";
        os<<"\n}\n";
    }

    inline BBHCandidate::index_t
    BBHCandidate::size() const noexcept {
        return size_;
    }
    inline BBHCandidate::container_tr const
    BBHCandidate::getCandidateList() {
        return candidates_;
    }
    inline BBHCandidate::container_tcr const
    BBHCandidate::getCandidateList() const {
        return candidates_;
    }

    inline BBHCandidate::score_t
    BBHCandidate::getBestScore() const noexcept {
        return currentBestScore_;
    }

}
#endif