#ifndef HOMOLOGY_INCLUDE_GUARD
#define HOMOLOGY_INCLUDE_GUARD 1

#include <utility>
#include <vector>
#include <cstddef>
#include <bitset>
#include <chrono>

#include "../threads/ThreadPool.hh"
#include "genx/Genome.hh"
#include "VariablesTypes.hh"
#include "genx/Gene.hh"
#include "genx/GenomesContainer.hh"
#include "bbh/BBHCandidatesContainer.hh"
#include "bbh/MinBBHContainer.hh"
#include <cmath>
#include <unordered_set>
#include <iomanip>

#include "kmers/KmerMapper.hh"
#include "ScoresContainer.hh"

#include "./../utils/FileWriter.hh"
#include "./../utils/StopWatch.hh"


/**
 * @file Homology.hh
 * @brief Definitions for the Homology class.
 */

 /**
  * @namespace homology
  * @brief Namespace containing definitions for homology computation related classes.
  */

namespace homology {

    using namespace score;

    /**
     * @class Homology
     * @brief Class for computing homology between genomes.
     */
    class Homology {
    private:

        using k_t = shared::kType;
        using index_t = shared::indexType;
        using score_t = shared::scoreType;
        using multiplicity_t = shared::multiplicityType;
        using minBBH_t = bbh::MinBBHContainer;



        using kmersContainer_t = kmers::KmersContainer;
        using kmersContainer_tr = const kmersContainer_t&;
        using kmersSet_tr = const kmersContainer_t::kmerSet_tr;

        using genome_t = genome::Genome;
        using genome_tr = genome_t&;
        using genome_tp = genome_t*;
        using gene_t = gene::Gene;
        using gene_tr = const gene_t&;
        using gene_tp = gene_t*;


        using BBHCandidate_t = bbh::BBHCandidate;
        using BBHCandidate_tp = BBHCandidate_t*;
        using BBHCandidate_tr = BBHCandidate_t&;

        using BBHcandidatesContainer_t = bbh::BBHCandidatesContainer;
        using BBHcandidatesContainer_tp = BBHcandidatesContainer_t*;
        using BBHcandidatesContainer_tr = BBHcandidatesContainer_t&;

        using thread_pt = threads::ThreadPool;
        using thread_ptp = thread_pt*;
        using thread_ptr = thread_pt&;


        k_t k_;
        utilities::FileWriter* fw;
        std::fstream outStream_;
        thread_ptp pool_;
        std::string inFile_;
        minBBH_t mins_;
        score_t similarityMinVal_;

        /**
         * @brief Calculates the similarity values for a row using the Generalized Jaccard index.
         *        Parallel computation is performed by sending each row as a task to the ThreadPool,
         *        ensuring no concurrency issues.
         * @param rowGenes The genes in the row.
         * @param colGenes The genes in the column.
         * @param bestRows The bestRows object containing candidates columns for Bidirectional Best Hit (BBH).
         * @param scores The container for storing similarity scores.
         */
        inline void calculateRow(
            genome_t::gene_ctr rowGenes, genome_t::gene_ctr colGenes,
            BBHcandidatesContainer_tr bestRows, ScoresContainer& scores
        ) const;


        /**
         * @brief Calculates the similarity values for a row using the Generalized Jaccard index.
         *        Parallel computation is performed by sending each row as a task to the ThreadPool,
         *        ensuring no concurrency issues. This function is specialized for cases where the genome
         *        is the same, resulting in fewer comparisons to be made.
         * @param colGene The genes in the column.
         * @param bestRows The bestRows object containing candidates columns for Bidirectional Best Hit (BBH).
         * @param scores The container for storing similarity scores.
         */
        inline void calculateRowSame(index_t genomeId, genome_t::gene_ctr colGene,
            BBHcandidatesContainer_tr bestRows, ScoresContainer& scores) const;

        /**
         * @brief Extracts Bidirectional Best Hits (BBH) using the similarity values calculated by calculateRow.
         * @param colGenes The genes in the column.
         * @param rowGenes The genes in the row.
         * @param candidates The container of BBH candidates (bestRows param.of calculateRow).
         * @param scores The container for storing similarity scores.
         */
        inline score_t
            checkForBBH(
                const genome_t::gene_ctr colGenes,
                const genome_t::gene_ctr rowGenes,
                BBHcandidatesContainer_tr candidates,
                ScoresContainer& scores
            );

        /**
         * @brief Extracts Bidirectional Best Hits (BBH) using the similarity values calculated by calculateRowSame.
         * @param genes The genes for which to extract BBH.
         * @param candidates The container of BBH candidates (bestRows param.of calculateRow).
         * @param scores The container for storing similarity scores.
         */
        inline void
            checkForBBHSame(
                const genome_t::gene_ctr genes,
                BBHcandidatesContainer_tr candidates,
                ScoresContainer& scores
            );


        /**
         * @brief Calculates the similarity between two genes using the Generalized Jaccard index.
         *        This function is used for initial filtering based on the total multiplicity of genes.
         * @param gene1 The first gene.
         * @param gene2 The second gene.
         * @return The similarity score between the two genes.
         */
        inline score_t
            calculateSimilarity(const gene_tr gene1, const gene_tr gene2) const;

        /**
         * @brief Calculates the similarity between two kmers containers using the Jaccard index.
         * @param gene1Container The kmers container of the first gene.
         * @param gene2Container The kmers container of the second gene.
         * @return The similarity score between the two kmers containers.
         */
        inline score_t
            calculateSimilarity(kmersContainer_tr gene1Container, kmersContainer_tr gene2Container) const;

        /**
         * @brief Calculates Bidirectional Best Hits (BBH) between genes of different genomes.
         * @param genome1 The first genome.
         * @param genome2 The second genome.
         */
        inline void calculateBidirectionalBestHitDifferentGenomes(genome_tr genome1, genome_tr genome2);

        /**
         * @brief Calculates Bidirectional Best Hits (BBH) between genes of the same genome.
         * @param genome The genome.
         */
        inline void calculateBidirectionalBestHitSameGenome(genome_tr genome);
    public:
        Homology() = delete;

        /**
         * @brief Constructs a Homology object with the specified parameters.
         * @param k The length of kmers.
         * @param fileName The name of the output file.
         * @param threadNumber The number of threads to use.
         */
        inline explicit Homology(k_t k, std::string fileName, ushort threadNumber, bool sensibilityParameter);


        /**
         * @brief Constructs a Homology object with the specified parameters.
         * @param k The length of kmers.
         * @param fileName The name of the output file.
         */
        inline explicit Homology(k_t k, std::string fileName, bool sensibilityParameter);

        Homology(const Homology&) = delete;
        Homology operator=(const Homology&) = delete;
        Homology(Homology&&) = delete;
        Homology& operator=(Homology&&) = delete;

        /**
         * @brief Destructor for Homology objects.
         */
        ~Homology() {
            fw->close(outStream_);
            delete fw;
            pool_->stop();
            delete(pool_);
        }

        /**
         * @brief Calculates Bidirectional Best Hit (BBH) for a set of genomes.
         * @param g The container of genomes.
         * @param mode The mode for recalculating kmers.
         */
        inline void calculateBidirectionalBestHit(genome::GenomesContainer& g, bool mode);
    };

    inline
        Homology::Homology(k_t k, std::string fileName, ushort threadNumber, bool sensibilityParameter)
        : k_(k) {
        if (k <= 0)
            throw std::runtime_error("k <= 0");
        std::cerr << "sensibility paramether homology builder: " << sensibilityParameter << std::endl;
        if (sensibilityParameter) {
            similarityMinVal_ = (2.0 / k);
        }
        else {
            similarityMinVal_ = 1.0 / (k * 2.0);
        }

        pool_ = new thread_pt(threadNumber);
        pool_->start();
        fw = new utilities::FileWriter("", fileName, ".net", false);
        outStream_ = fw->openAppend();
    }

    inline
        Homology::Homology(k_t k, std::string fileName, bool sensibilityParameter)
        : k_(k) {
        if (k <= 0)
            throw std::runtime_error("k <= 0");
        if (sensibilityParameter) {
            similarityMinVal_ = (2.0 / k);
        }
        else {
            similarityMinVal_ = 1.0 / (k * 2.0);
        }
        pool_ = new thread_pt();
        pool_->start();
        fw = new utilities::FileWriter("", fileName, ".net", false);
        outStream_ = fw->openAppend();
    }

    // 2 generalized Jaccard similarity
    // all kmers must be calculated before


    inline Homology::score_t
        Homology::calculateSimilarity(const gene_tr gene1, const gene_tr gene2) const {
        return
            (
                gene1.getAlphabetLength() < gene2.getCut()
                || gene2.getAlphabetLength() < gene1.getCut()
                ) ?
            0
            :
            calculateSimilarity(
                gene1.getKmersNum() < gene2.getKmersNum() ? *gene1.getKmerContainer() : *gene2.getKmerContainer(),
                gene1.getKmersNum() < gene2.getKmersNum() ? *gene2.getKmerContainer() : *gene1.getKmerContainer()
            );
    }

    inline Homology::score_t
        Homology::calculateSimilarity(kmersContainer_tr shortestContainer, kmersContainer_tr longestContainer) const {

        index_t longestBiggerKey = longestContainer.getBiggerKey();

        kmersSet_tr shortestSet = shortestContainer.getKmerSet();
        kmersSet_tr longestSet = longestContainer.getKmerSet();

        std::size_t num = 0;
        std::size_t den = 0;

        multiplicity_t currentShortestMultiplicity = 0;
        multiplicity_t currentLongestMultiplicity = 0;

        auto shortestSetBegin = shortestSet.begin();
        auto longestSetBegin = longestSet.begin();
        auto shortestSetEnd = shortestSet.end();
        auto longestSetEnd = longestSet.end();


        while (shortestSetBegin != shortestSetEnd && longestSetBegin != longestSetEnd) {

            index_t shortestKey = shortestSetBegin->first;
            index_t longestKey = longestSetBegin->first;

            if (shortestKey > longestBiggerKey) {
                break;
            }

            if (shortestKey < longestKey) {
                ++shortestSetBegin;
            }
            else if (shortestKey > longestKey) {

                ++longestSetBegin;
            }
            else {


                multiplicity_t currentKmerValue = shortestSetBegin->second;
                multiplicity_t longestVal = longestSetBegin->second;

                num += (currentKmerValue < longestVal ? currentKmerValue : longestVal);
                den += (currentKmerValue < longestVal ? longestVal : currentKmerValue);

                currentShortestMultiplicity += currentKmerValue;
                currentLongestMultiplicity += longestVal;

                ++shortestSetBegin;
                ++longestSetBegin;
            }
        }

        // getMultiplicityNumber() returns the number of all possible kmers. it is computed during the construction of the kmer container
        index_t shortestMultiplityNumber = shortestContainer.getMultiplicityNumber();
        index_t longestMultiplityNumber = longestContainer.getMultiplicityNumber();

        return
            (
                (
                    ((1.0 * num) / shortestMultiplityNumber) < similarityMinVal_
                    ) ||
                (
                    ((1.0 * num) / longestMultiplityNumber) < similarityMinVal_
                    )
                ) ?
            0 : (
                1.0 * num / (den + ((shortestMultiplityNumber - currentShortestMultiplicity) + (longestMultiplityNumber - currentLongestMultiplicity)))
                );
    }



    void Homology::calculateBidirectionalBestHit(genome::GenomesContainer& gc, bool mode) {
        // std::cerr<<"\npre resize";
        mins_.resize(gc.size());
        // std::cerr<<"\npost resize";

        // std::cerr<<"\nsimilarityMinVal_: "<<similarityMinVal_<<"\n";
        if (mode) {
            genome::GenomesContainer::genome_ctr genomes = gc.getGenomes();

            auto& pool = *pool_;

            for (auto rowGenome = genomes.begin(); rowGenome != genomes.end(); ++rowGenome) {
                // std::cerr<<"\npre mapper";

                kmers::KmerMapper mapper;

                auto& rowRef = *rowGenome;
                // std::cerr<<"\npre mapping";
                rowRef.createAndCalculateAllKmers(k_, mapper);
                // std::cerr<<"\npost mapping";

                auto colGenome = rowGenome;
                ++colGenome;

                for (; colGenome != genomes.end(); ++colGenome) {
                    colGenome->createAndCalculateAllKmers(k_, mapper);
                    calculateBidirectionalBestHitDifferentGenomes(*colGenome, rowRef);
                    colGenome->deleteAllKmers(pool);
                }


                rowRef.deleteAllKmers(pool);
            }
            // std::cerr<<"\ncomputing mins";
            mins_.computeMins(pool);
            // std::cerr<<"\npost computing mins";
            mins_.print();

            for (auto rowGenome = genomes.begin(); rowGenome != genomes.end(); ++rowGenome) {
                kmers::KmerMapper mapper;
                auto& rowRef = *rowGenome;
                rowRef.createAndCalculateAllKmers(k_, mapper);
                calculateBidirectionalBestHitSameGenome(rowRef);
                rowRef.deleteAllKmers(pool);
            }

        }
        else {

            genome::GenomesContainer::genome_ctr genomes = gc.getGenomes();

            // Create and calculate kmers for each genome
            {
                kmers::KmerMapper mapper;
                for (auto genome = genomes.begin(); genome != genomes.end(); ++genome)
                    genome->createAndCalculateAllKmers(k_, mapper);
            }

            auto& pool = *pool_;

            // Compare each genome with every other genome to find BBH
            for (auto rowGenome = genomes.begin(); rowGenome != genomes.end(); ++rowGenome) {
                auto& rowRef = *rowGenome;

                auto colGenome = rowGenome;
                ++colGenome;

                for (; colGenome != genomes.end(); ++colGenome) {
                    calculateBidirectionalBestHitDifferentGenomes(*colGenome, rowRef);
                }


            }
            // std::cerr<<"\npre computing mins";

            mins_.computeMins(pool);
            // std::cerr<<"\npost computing mins";

            mins_.print();
            for (auto rowGenome = genomes.begin(); rowGenome != genomes.end(); ++rowGenome) {
                auto& rowRef = *rowGenome;
                calculateBidirectionalBestHitSameGenome(rowRef);
                rowRef.deleteAllKmers(pool);
            }

        }



    }


    // colGenome, rowGenome
    inline void
        Homology::calculateBidirectionalBestHitDifferentGenomes(
            genome_tr colGenome, genome_tr rowGenome
        ) {
        // std::cerr<<"\ncomparing different";
        std::cerr << "\nComparing different genomes <col, row> " << colGenome.getId() << " - " << rowGenome.getId();

        // genes in genome1 rapresents the width of the matrix (cols), genes in genome2 rapresents the height(rows)
        genome_t::gene_ctr colGenes = colGenome.getGenes();
        genome_t::gene_ctr rowGenes = rowGenome.getGenes();


        BBHcandidatesContainer_t bestRows(rowGenes.size(), colGenes.size());

        ScoresContainer scores(rowGenes.size(), colGenes.size());

        // per la crezione della comparazione modificare qui il valore passatto usando "startCol"
        calculateRow(
            rowGenes, colGenes,
            bestRows, scores
        );

        score_t minBBH = checkForBBH(
            colGenes, rowGenes,
            bestRows,
            scores
        );

        mins_.setVal(rowGenome.getId(), colGenome.getId(), minBBH);
    }



    // inline Homology::containerTypePointer
    inline void
        Homology::calculateBidirectionalBestHitSameGenome(
            genome_tr genome
        ) {
        std::cerr << "\nComparing same genomes " << genome.getId() << " - " << genome.getId();
        // std::cerr<<"\ncomparing same";
        genome_t::gene_ctr genes = genome.getGenes();
        // genome_t::gene_ctr rowGenes = genome.getGenes();

        BBHcandidatesContainer_t bestRows(genome.size(), genome.size());
        ScoresContainer scores(genome.size(), genome.size());

        calculateRowSame(
            genome.getId(),
            genes,
            bestRows, scores
        );

        checkForBBHSame(
            genes,
            bestRows,
            scores
        );
    }


    inline void
        Homology::calculateRowSame(
            index_t genomeId,
            genome_t::gene_ctr genes,
            BBHcandidatesContainer_tr bestRows, ScoresContainer& scores
        ) const {
        thread_ptr poolRef = *pool_;
        score_t minScore = mins_.getMin(genomeId);

        for (index_t row = 0; row < genes.size(); ++row) {
            poolRef.execute(
                [row, &scores, this, &genes, &bestRows, minScore] {
                    for (index_t col = row + 1; col < genes.size(); ++col) {
                        const auto& g = genes[row];
                        score_t currentScore = calculateSimilarity(g, genes[col]);
                        if (currentScore >= minScore) {
                            scores.setScoreAt(row, col, currentScore);
                            bestRows.addCandidate(row, currentScore, col);
                        }
                        // scores.setScoreAt(row, col, currentScore);
                        // bestRows.addCandidate(row, currentScore, col);
                    }
                }
            );
        }
        // poolRef.waitTasks();

        while (!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    inline void
        Homology::calculateRow(
            genome_t::gene_ctr rowGenes, genome_t::gene_ctr colGenes,
            BBHcandidatesContainer_tr bestRows, ScoresContainer& scores) const {

        thread_ptr poolRef = *pool_;

        for (index_t row = 0; row < rowGenes.size(); ++row) {
            // gene_tr rowGene = rowGenes.at(row);
            poolRef.execute(
                [row, &scores, this, &colGenes, &bestRows, &rowGenes] {
                    gene_tr rowGene = rowGenes[row];
                    for (index_t col = 0; col < colGenes.size(); ++col) {
                        score_t currentScore = calculateSimilarity(rowGene, colGenes[col]);
                        if (currentScore > 0) {
                            scores.setScoreAt(row, col, currentScore);
                            bestRows.addCandidate(row, currentScore, col);
                        }
                    }
                }
            );
        }
        // poolRef.waitTasks();
        while (!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }



    inline Homology::score_t
        Homology::checkForBBH(
            const genome_t::gene_ctr colGenes, const genome_t::gene_ctr rowGenes,
            BBHcandidatesContainer_tr candidates,
            ScoresContainer& scores
        ) {

        score_t sharedMin = 1;
        std::mutex minMutex;

        auto& poolRef = *pool_;

        // auto matchp = candidates.getPossibleMatch(rowGenes.size());
        auto matchp = candidates.getPossibleMatch(colGenes.size(), poolRef);

        auto& match = *matchp;

        // BBHcandidatesContainer_t::set_tr matchRef = *match;
        // bbh::BidirectionalBestHitContainer& bbhContainerRef = *bbh;
        // passo per tutte le colonne "candidate"

        for (auto col = match.begin(); col != match.end(); ++col) {
            const auto& currentColRef = *col;


            poolRef.execute(
                [&currentColRef, &colGenes, &rowGenes, &scores, &candidates, this, &sharedMin, &minMutex] {

                    auto& fwRef = *fw;
                    score_t bestScore = -1;


                    std::unordered_set<index_t> currentBestIndexs;

                    // index_t colGeneId = currentColRef.first;
                    index_t colGeneId = currentColRef;;
                    gene_tr currentColGene = colGenes[colGeneId];
                    // estrae le migliori righe per la colonna corrente
                    // e li memorizza in current best indexs

                    for (index_t row = 0; row < rowGenes.size(); ++row) {
                        score_t currentScore = scores.getScoreAt(row, colGeneId);

                        if (currentScore > bestScore && currentScore > 0.0) {
                            bestScore = currentScore;
                            currentBestIndexs.clear();
                            currentBestIndexs.emplace(row);
                        }
                        else if (currentScore == bestScore && currentScore > 0.0) {
                            currentBestIndexs.emplace(row);
                        }
                    }

                    // passa per tutte le righe con il punteggio migliore
                    // e se quel punteggio è il migliore anche per la riga
                    // crea il bbh
                    if (bestScore > 0.0) {

                        score_t minBBH = 2;

                        index_t currentColGeneFileLine = currentColGene.getGeneFilePosition();

                        for (auto index = currentBestIndexs.begin(); index != currentBestIndexs.end(); ++index) {
                            index_t currentIndex = *index;

                            if (bestScore == candidates.getBestScoreForCandidate(currentIndex)) {
                                fwRef.write(
                                    std::to_string(
                                        rowGenes[currentIndex].getGeneFilePosition()
                                    ) + "," +
                                    std::to_string(
                                        currentColGeneFileLine
                                    ) + "," +
                                    std::to_string(bestScore)
                                    , outStream_);
                                minBBH = bestScore < minBBH ? bestScore : minBBH;
                            }
                        }
                        if (minBBH < sharedMin) {
                            std::unique_lock<std::mutex> lock(minMutex);
                            sharedMin = minBBH < sharedMin ? minBBH : sharedMin;
                        }
                    }


                }
            );
        }
        // poolRef.waitTasks();

        while (!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // poolRef.waitTasks();

        delete matchp;
        return sharedMin;
    }

    inline void
        Homology::checkForBBHSame(
            const genome_t::gene_ctr genes,
            BBHcandidatesContainer_tr candidates,
            ScoresContainer& scores
        ) {
        auto& poolRef = *pool_;

        auto matchp = candidates.getPossibleMatch(genes.size(), poolRef);
        // auto matchp = candidates.getPossibleMatch(genes.size());
        auto& match = *matchp;
        // BBHcandidatesContainer_t::set_tr matchRef = *match;

        // bbh::BidirectionalBestHitContainer& bbhContainerRef = *bbh;

        // passo per tutte le colonne "candidate"
        for (auto col = match.begin(); col != match.end(); ++col) {
            const auto& currentColRef = *col;
            poolRef.execute(
                [&currentColRef, &genes, &scores, &candidates, this] {
                    auto& fwRef = *fw;
                    score_t bestScore = -1;

                    std::unordered_set<index_t> currentBestIndexs;
                    // index_t colGeneId = currentColRef.first;
                    index_t colGeneId = currentColRef;
                    gene_tr currentColGene = genes[colGeneId];
                    // estrae le migliori righe per la colonna corrente
                    // e li memorizza in current best indexs

                    // le prime righe fino alla diagonale
                    for (index_t row = 0; row < colGeneId; ++row) {
                        score_t currentScore = scores.getScoreAt(row, colGeneId);

                        if (currentScore > bestScore && currentScore > 0.0) {
                            bestScore = currentScore;
                            currentBestIndexs.clear();
                            currentBestIndexs.insert(row);
                        }
                        else if (currentScore == bestScore && currentScore > 0.0) {
                            currentBestIndexs.insert(row);
                        }
                    }

                    // passa per tutte le righe con il punteggio migliore
                    // e se quel punteggio è il migliore anche per la riga
                    // crea il bbh

                    if (bestScore > 0.0) {
                        index_t currentColGeneFileLine = currentColGene.getGeneFilePosition();
                        for (auto index = currentBestIndexs.begin(); index != currentBestIndexs.end(); ++index) {
                            index_t currentIndex = *index;

                            if (bestScore == candidates.getBestScoreForCandidate(currentIndex)) {
                                fwRef.write(
                                    std::to_string(
                                        genes[currentIndex].getGeneFilePosition()
                                    ) + "," +
                                    std::to_string(
                                        currentColGeneFileLine
                                    ) + "," +
                                    std::to_string(bestScore)
                                    , outStream_);

                            }
                        }
                    }
                }
            );
        }
        // poolRef.waitTasks();

        while (!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        delete matchp;
    }

}

#endif