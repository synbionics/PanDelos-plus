#ifndef VARIABLES_TYPES_INCLUDE_GUARD
#define VARIABLES_TYPES_INCLUDE_GUARD 1

#include <string>
#include <cstdint>

/**
 * @brief Macro to enable development mode.
 * 
 * When set to 1, enables development mode. 
 * Comment out or set to 0 to disable.
 */
// #define DEV_MODE 1


/**
 * @brief Namespace containing shared variable types.
 */
namespace shared {
    /**
     * @brief Type alias for indices.
     */
    using indexType = std::size_t;

    /**
     * @brief Type alias for sequences.
     */
    using sequenceType = std::string;

    /**
     * @brief Type alias for sub-sequences.
     */
    using subSequenceType = std::string;

    /**
     * @brief Type alias for kmers.
     */
    using kType = unsigned short;

    /**
     * @brief Type alias for multiplicity counts.
     */
    using multiplicityType = std::size_t;

    /**
     * @brief Type alias for scores.
     */
    using scoreType = double;

    /**
     * @brief Constant indicating a cut value.
     *
     * This constant represents a cut-off value used in certain calculations.
     */
    const size_t cut = 4;
}


#endif