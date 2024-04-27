#ifndef SCORES_CONTAINERE_INCLUDE_GUARD
#define SCORES_CONTAINERE_INCLUDE_GUARD 1

#include <vector>

#include "VariablesTypes.hh"



/**
 * @file ScoresContainer.hh
 * @brief Definitions for the ScoresContainer class.
 */

/**
 * @namespace score
 * @brief Namespace containing definitions for score related classes.
 */
namespace score {

    /**
     * @class ScoresContainer
     * @brief Represents a container for storing scores associated with rows and columns.
     */
    class ScoresContainer {
        private:
            using score_t = shared::scoreType; 
            using index_t = shared::indexType;
            
            using cols_t = std::vector<score_t>;
            using scores_t = std::vector<cols_t>;

            index_t rows_;
            index_t cols_;

            // dimensione massima effettiva [0, capacity)
            index_t capacity_;
            
            // invece di allocare una matrice rows_*cols_ si crea un array dove tutte le righe
            // sono una di seguito all'altra
            scores_t scores_;
        public:

            /**
             * @brief Deleted default constructor to prevent instantiation without parameters.
             */
            ScoresContainer() = delete;
            
            /**
             * @brief Constructs a ScoresContainer object with the specified number of rows and columns.
             * 
             * @param rowsNumber Number of rows.
             * @param colsNumber Number of columns.
             */
            inline explicit ScoresContainer(index_t rowsNumber, index_t colsNumber);

            ScoresContainer(const ScoresContainer& other) = delete;
            ScoresContainer(const ScoresContainer&& other) = delete;
            ScoresContainer& operator=(const ScoresContainer& other) = delete;
            ScoresContainer& operator=(const ScoresContainer&& other) = delete;



            /**
             * @brief Sets the score at the specified row and column.
             * 
             * @param rowNumber Row index.
             * @param colNumber Column index.
             * @param newScore New score to set.
             */
            inline void setScoreAt(index_t rowNumber, index_t colNumber, score_t newScore);

            /**
             * @brief Retrieves the score at the specified row and column.
             * 
             * @param rowNumber Row index.
             * @param colNumber Column index.
             * @return Score at the specified row and column.
             */
            inline score_t getScoreAt(index_t rowNumber, index_t colNumber);

            /**
             * @brief Retrieves the capacity of the container.
             * 
             * @return Capacity of the container.
             */
            inline index_t getCapacity() const;
            
            /**
             * @brief Default destructor.
             */
            inline ~ScoresContainer() = default;
    };


    inline
    ScoresContainer::ScoresContainer(index_t rowsNumber, index_t colsNumber)
    : rows_(rowsNumber), cols_(colsNumber), capacity_(cols_*rows_) {
        scores_.reserve(rowsNumber);

        for(index_t i = 0; i < rowsNumber; ++i) {
            cols_t cols;
            cols.reserve(colsNumber);
            for(index_t j = 0; j < colsNumber; ++j) {
                cols.emplace_back(
                    0
                );
            }
            scores_.emplace_back(
                std::move(
                    std::move(
                        cols
                    )
                )
            );
        }
    };

    inline void
    ScoresContainer::setScoreAt(index_t rowNumber, index_t colNumber, score_t newScore) {
        scores_[rowNumber][colNumber] = newScore;
    }

    inline ScoresContainer::score_t
    ScoresContainer::getScoreAt(index_t rowNumber, index_t colNumber) {
        return scores_[rowNumber][colNumber];
    }

    inline ScoresContainer::index_t
    ScoresContainer::getCapacity() const {
        return capacity_;
    }
}
#endif