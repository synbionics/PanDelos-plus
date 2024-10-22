#ifndef KMER_MAPPER_INCLUDE_GUARD
#define KMER_MAPPER_INCLUDE_GUARD 1

#include <cstddef>
#include <iostream>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include "../VariablesTypes.hh"


/**
 * @file KmerMapper.hh
 * @brief Definitions for the KmerMapper class.
 */

 /**
  * @namespace kmers
  * @brief Namespace containing definitions for kmer related classes.
  */
namespace kmers {

    /**
     * @brief Class for mapping kmers to indices.
     *
     * This class maps kmers (substrings) to unique indices.
     */
    class KmerMapper {
    private:
        using index_t = shared::indexType;
        using subsequence_t = shared::subSequenceType;
        using subsequence_tr = subsequence_t&;
        using map_t = __gnu_pbds::gp_hash_table<subsequence_t, index_t>;
        index_t nextIndex_;
        map_t map_;

    public:
        /**
         * @brief Default constructor.
         */
        inline explicit KmerMapper() noexcept;
        /**
         * @brief Maps a subsequence to an index.
         *
         * @param str The subsequence to be mapped.
         * @return The index associated with the subsequence.
         */

        KmerMapper(const KmerMapper& other) = delete;
        KmerMapper(const KmerMapper&& other) = delete;
        KmerMapper& operator=(const KmerMapper& other) = delete;
        KmerMapper& operator=(const KmerMapper&& other) = delete;


        inline index_t mapAndGetIndex(const subsequence_tr str) noexcept;

#ifdef DEV_MODE
        /**
         * @brief Prints the mapping to an output stream.
         *
         * @param os The output stream to print to.
         */
        inline void print(std::ostream& os) const;
#endif
        /**
         * @brief Gets the size of the mapping.
         *
         * @return The number of elements in the mapping.
         */
        inline size_t size() const noexcept;

        /**
         * @brief Default destructor.
         */
        inline ~KmerMapper() = default;
    };

    /**
     * @brief Default constructor implementation.
     */
    inline KmerMapper::KmerMapper() noexcept : nextIndex_(0) {
#ifdef xTOR_DEBUG
        std::cerr << "\nCtor KmerMapper::KmerMapper";
#endif
    }

    /**
     * @brief Maps a subsequence to an index.
     *
     * @param str The subsequence to be mapped.
     * @return The index associated with the subsequence.
     */
    inline KmerMapper::index_t
        KmerMapper::mapAndGetIndex(const subsequence_tr str) noexcept {
        auto elem = map_.find(str);
        if (elem != map_.end())
            return elem->second;
        map_.insert(
            std::move(
                std::make_pair(
                    str, nextIndex_
                )
            )
        );
        index_t ret = nextIndex_;
        ++nextIndex_;
        return ret;
    }

#ifdef DEV_MODE
    /**
     * @brief Prints the mapping to an output stream.
     *
     * @param os The output stream to print to.
     */
    inline void
        KmerMapper::print(std::ostream& os) const {
        for (auto it = map_.begin(); it != map_.end(); ++it) {
            os << "\n" << it->first << " : " << it->second;
        }
}
#endif

    /**
     * @brief Gets the size of the mapping.
     *
     * @return The number of elements in the mapping.
     */
    inline size_t
        KmerMapper::size() const noexcept {
        return map_.size();
    }
}

#endif