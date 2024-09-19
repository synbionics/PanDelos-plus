#ifndef MIN_BBH_CONTAINER_INCLUDE_GUARD
#define MIN_BBH_CONTAINER_INCLUDE_GUARD 1


#include "../VariablesTypes.hh"
#include "../../threads/ThreadPool.hh"
#include <vector>
#include <iostream>
#include <functional>
#include <chrono>
#include <cstddef>

namespace bbh {

    class MinBBHContainer {
    private:
        using index_t = shared::indexType;
        using score_t = shared::scoreType;
        using line_t = std::vector<score_t>;
        using line_ct = std::vector<line_t>;
        using pool_t = threads::ThreadPool;
        using pool_tr = pool_t&;

        line_ct halfMatrix_;
        index_t rows_;
        line_t mins_;

        inline score_t getVal(const index_t row, const index_t col) const;
    public:

        inline explicit MinBBHContainer();
        inline void resize(const index_t rows);
        ~MinBBHContainer();
        inline void print() const;
        inline void setVal(const index_t row, const index_t col, const score_t min);
        inline void computeMins(pool_tr pool);
        inline score_t getMin(const index_t row) const;

    };

    inline MinBBHContainer::MinBBHContainer() : rows_(0) {}

    inline void
        MinBBHContainer::resize(const index_t rows) {
        // std::cerr<<"resizing, new rows: "<<rows<<"\n";
        index_t maxCols = rows - 1;
        // std::cerr<<"resizing, new maxCols: "<<maxCols<<"\n";
        for (index_t i = 0; i < rows; ++i) {
            halfMatrix_.push_back(
                line_t(maxCols - i, 0)
            );
        }
        rows_ = rows;
        mins_ = line_t(rows, 0);
    }


    MinBBHContainer::~MinBBHContainer() {}

    inline void
        MinBBHContainer::print() const {

        std::cerr << "\n";
        for (size_t i = 0; i < rows_; i++) {
            std::cerr << "\n" << i << ": ";
            for (auto c = halfMatrix_[i].begin(); c != halfMatrix_[i].end(); ++c) {
                std::cerr << "| " << *c << " ";
            }
            std::cerr << "|";
        }
        std::cerr << "\n";
        for (size_t i = 0; i < rows_; i++) {
            std::cerr << "| " << mins_[i] << " ";
        }
        std::cerr << "|\n";
    }



    inline void
        MinBBHContainer::setVal(const index_t row, const index_t col, const score_t min) {
        index_t correctCol = col - 1 - row;
        halfMatrix_[row][correctCol] = min;
    }

    inline MinBBHContainer::score_t
        MinBBHContainer::getVal(const index_t row, const index_t col) const {
        index_t correctCol = col - 1 - row;
        return halfMatrix_[row][correctCol];
    }

    inline void
        MinBBHContainer::computeMins(pool_tr pool) {

        index_t effectiveRows = rows_ - 1;
        if (effectiveRows == 0) {
            mins_[0] = 0;
            return;
        }

        pool.execute(
            [this] {
                auto c = halfMatrix_[0].begin();
                score_t currentMin = *c;
                ++c;
                for (; c != halfMatrix_[0].end(); ++c) {
                    currentMin = *c < currentMin ? *c : currentMin;
                }
                mins_[0] = currentMin;
            }
        );

        for (index_t r = 1; r < effectiveRows; ++r) {

            pool.execute(
                [this, r] {
                    auto c = halfMatrix_[r].begin();
                    score_t currentMin = *c;
                    ++c;
                    for (; c != halfMatrix_[r].end(); ++c) {
                        currentMin = *c < currentMin ? *c : currentMin;
                    }
                    index_t p = r - 1;
                    for (; p > 0; --p) {
                        score_t tmp = getVal(p, r);
                        currentMin = tmp < currentMin ? tmp : currentMin;
                    }
                    // p = 0
                    score_t tmp = getVal(p, r);
                    currentMin = tmp < currentMin ? tmp : currentMin;
                    mins_[r] = currentMin;
                }
            );



        }

        // last row
        pool.execute(
            [this, effectiveRows] {
                index_t p = effectiveRows - 1;
                score_t currentMin = getVal(p, effectiveRows);
                --p;
                for (; p > 0; --p) {
                    score_t tmp = getVal(p, effectiveRows);
                    currentMin = tmp < currentMin ? tmp : currentMin;
                }
                // p = 0
                score_t tmp = getVal(p, effectiveRows);
                currentMin = tmp < currentMin ? tmp : currentMin;
                mins_[effectiveRows] = currentMin;
            }
        );
        while (!pool.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

    }
    inline MinBBHContainer::score_t
        MinBBHContainer::getMin(const index_t row) const {
        return mins_[row];
    }
}

#endif