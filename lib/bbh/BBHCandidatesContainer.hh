#ifndef BBH_CANDIDATES_CONTAINER_INCLUDE_GUARD
#define BBH_CANDIDATES_CONTAINER_INCLUDE_GUARD 1

#include <cstddef>
#include <utility>
#include <iostream>
#include <mutex>
#include <vector>
#include <unordered_set>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include "../VariablesTypes.hh"
#include "BBHCandidate.hh"
#include "../../threads/ThreadPool.hh"



/**
 * @file BBHCandidatesContainer.hh
 * @brief Definitions for the BBHCandidatesContainer class.
 */

/**
 * @namespace bbh
 * @brief Namespace containing definitions for Best Bidirectional Hits (BBH) related classes.
 */

namespace bbh{

    /**
     * @class BBHCandidatesContainer
     * @brief Represents a container for managing sets of BBHCandidates.
     */
    class BBHCandidatesContainer {
        private:

            using index_t = shared::indexType;
            using score_t = shared::scoreType;

            using BBHCandidate_t = bbh::BBHCandidate;
            using BBHCandidate_tp = BBHCandidate_t*;
            using BBHCandidatesSet = std::unordered_set<index_t>;
            
            using candidates_t = std::vector<BBHCandidate_t>;
            
            index_t capacity_;
            candidates_t candidates_;
            using set_t = __gnu_pbds::gp_hash_table<index_t, BBHCandidatesSet>;
            using setShared_t = std::vector<bool>;
            using set_mutexs = std::vector<std::mutex>;

        public:
            using setShared_tp = setShared_t*;
            using set_tp = set_t*;
            using set_tr = set_t&;
            using setShared_tr = setShared_t&;
        

        public:
            using BBHCandidate_tr = BBHCandidate_t&;
            using BBHCandidate_tm = BBHCandidate_t&&;
            using BBHCandidatesSetReference = BBHCandidatesSet&;
        private:
            using matchSet_t = std::pair<index_t, set_tp>;

        public:
            using matchSet_tp = matchSet_t*;

            /**
             * @brief Constructs a BBHCandidatesContainer object with specified capacity and total number of columns.
             * 
             * @param capacity Capacity of the container.
             * @param totalCols Total number of columns.
             */
            inline explicit BBHCandidatesContainer(const index_t capacity, const index_t totalCols);


            BBHCandidatesContainer(const BBHCandidatesContainer&) = delete;
            BBHCandidatesContainer(BBHCandidatesContainer&&) = delete;
            BBHCandidatesContainer& operator=(const BBHCandidatesContainer&) = delete;
            BBHCandidatesContainer& operator=(BBHCandidatesContainer&&) = delete;
            
            /**
             * @brief Default destructor.
             */
            ~BBHCandidatesContainer() = default;
            
            /**
             * @brief Adds a candidate to the container at the specified index with the given score and new index.
             * 
             * @param candidateIndex Index of the candidate.
             * @param newScore New score of the candidate.
             * @param newIndex New index of the candidate.
             */
            inline void addCandidate(const index_t candidateIndex, const score_t newScore, const index_t newIndex);
            
            
            /**
             * @brief Retrieves the best score among the candidates for the specified index.
             * 
             * @param candidateIndex Index of the candidate.
             * @return Best score among the candidates for the specified index.
             */
            inline score_t getBestScoreForCandidate(const index_t candidateIndex);
            
            /**
             * @brief Prints the container and its contents to the output stream.
             * 
             * @param os Reference to the output stream.
             */
            inline void print(std::ostream& os) const;
            
            /**
             * @brief Retrieves a pointer to a map containing possible matches between candidate indexes and sets of column indexes.
             * 
             * @return Pointer to the map of possible matches.
             */
            inline set_tp getPossibleMatch(size_t maxSize) const;
            inline BBHCandidatesSet* getPossibleMatch(size_t maxSize, threads::ThreadPool& pool) const;
            
            /**
             * @brief Retrieves the capacity of the container.
             * 
             * @return Capacity of the container.
             */
            inline index_t getCapacity() const;

            /**
             * @brief Retrieves a reference to the BBHCandidate at the specified index.
             * 
             * @param id Index of the BBHCandidate.
             * @return Reference to the BBHCandidate at the specified index.
             */
            inline BBHCandidate_tr getCandidateAt(const index_t id);
    };

    inline
    BBHCandidatesContainer::BBHCandidatesContainer(const index_t capacity, const index_t totalCols)
    : capacity_(capacity){

        candidates_.reserve(capacity);
        for(index_t i = 0; i < capacity; ++i) {
            BBHCandidate_t candidate(totalCols);
            candidates_.emplace_back(
                std::move(candidate)
            );
        }
    }

    inline void
    BBHCandidatesContainer::addCandidate(const index_t candidateIndex, const score_t newScore, const index_t newIndex) {
        candidates_[candidateIndex].addCandidate(newScore, newIndex);
    }

    inline BBHCandidatesContainer::index_t
    BBHCandidatesContainer::getCapacity() const {
        return capacity_;
    }

    inline void
    BBHCandidatesContainer::print(std::ostream& os) const {
        for(index_t i = 0; i < capacity_; ++i) {
            os<<"\nCandidates for index "<<i<<":";
            candidates_[i].print(os);
        }
    }

    // returned obj is not deallocated, return std::pair* with match max index, and a list og pairs, where:
    // - first contains colums (secondary indexes)
    // - second contains cadidates rows (main indexes)
    inline BBHCandidatesContainer::BBHCandidatesSet*
    BBHCandidatesContainer::getPossibleMatch(size_t maxSize, threads::ThreadPool& pool) const {
        
        setShared_t bools(maxSize, false);
        // setShared_tr mapRef = *map;
        set_mutexs mutex_v(maxSize);

        for(index_t i = 0; i < capacity_; ++i){

            pool.execute(
                [i, this, &bools, &mutex_v] {
                    const auto& list = candidates_[i].getCandidateList();
                    for(auto k = list.begin(); k != list.end(); ++k) {
                        index_t key = *k;
                        if(!bools[key]) {
                            std::unique_lock<std::mutex> lock(mutex_v[key]);
                            if(!bools[key])
                                bools[key] = true;
                        }
                    }
                }
            );
        }

        while(!pool.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        BBHCandidatesSet* map = new BBHCandidatesSet();
        BBHCandidatesSet& mapRef = *map;
        mapRef.reserve(maxSize);
        for(index_t i = 0; i < maxSize; ++i){
            if(bools[i])
                mapRef.insert(i);
        }
        return map;
    }
    inline BBHCandidatesContainer::set_tp
    BBHCandidatesContainer::getPossibleMatch(size_t maxSize) const {
        
        set_tp map = new set_t();
        set_tr mapRef = *map;
        
        for(index_t i = 0; i < capacity_; ++i){
            
            const auto& list = candidates_[i].getCandidateList();
            for(auto k = list.begin(); k != list.end(); ++k) {
                index_t key = *k;
                auto mapIt = mapRef.find(key);
                if(mapIt == mapRef.end()) {
                    BBHCandidatesSet set;
                    set.reserve(maxSize);
                    set.insert(i);
                    mapRef.insert(
                        std::move(
                            std::make_pair(key, std::move(set))
                        )
                    );
                } else
                    mapIt->second.insert(i);
            }
        }
        return map;
    }

    inline BBHCandidatesContainer::BBHCandidate_tr
    BBHCandidatesContainer::getCandidateAt(const index_t id) {
        return candidates_[id];
    }

    inline BBHCandidatesContainer::score_t
    BBHCandidatesContainer::getBestScoreForCandidate(const index_t candidateIndex) {
        return candidates_[candidateIndex].getBestScore();
    }

}


#endif