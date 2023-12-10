#include <utility>

#include "./Genome.hh"
#include "./Gene.hh"

#ifndef HOMOLOY_DETECTOR_INCLUDE_GUARD
#define HOMOLOY_DETECTOR_INCLUDE_GUARD

#define DEBUG 1
// #define DEBUG 2

namespace homology_detector{
    class HomologyDetector {
        private:

            using kType = const unsigned short;
            using genomeType = genome::Genome;
            using genomeTypeReference = genomeType&;
            using geneType = gene::Gene;
            using geneTypeReference = geneType&;
        // ser CH candidateHomologous

            kType k_;

        public:
            HomologyDetector() = delete;
            HomologyDetector(kType k);
            void selectCandidatePairs();

            // calculate similarity on a given pair of genome
            void calculateSimilarity(genomeTypeReference genome1, genomeTypeReference genome2);

            // calculate similarity on a given pair of gene
            double calculateSimilarity(geneTypeReference gene1, geneTypeReference gene2);

            // calculate bidirectional best hit
            void calculateBidirectional();

            // sequence duplication within the same genome -> parologous
            void searchParalogy(genomeTypeReference genome);
            
            // sequence duplication between different genomes in the same lineage
            // ("vertical" transmittion) -> orthologous
            void searchOrtology();

            ~HomologyDetector();
    };
    HomologyDetector::HomologyDetector(kType k) : k_{k} {
        #ifdef DEBUG
        std::cerr<<"\nDetector created, k="<<k_<<"\n";
        #endif
    }

    HomologyDetector::~HomologyDetector(){
    }

    // 1 select candidates
    void HomologyDetector::selectCandidatePairs(){

    }
    // 2 generalized Jaccard similarity
    void HomologyDetector::calculateSimilarity(genomeTypeReference genome1, genomeTypeReference genome2){

    }

    // 2 generalized Jaccard similarity
    double HomologyDetector::calculateSimilarity(geneTypeReference gene1, geneTypeReference gene2){

        gene1.createNewKmersIfNotExists(k_);
        gene2.createNewKmersIfNotExists(k_);
        gene1.calculateKmers();
        gene2.calculateKmers();
        #ifdef DEBUG
        std::cerr<<"\nkmers created";
        gene1.getKmerHandler()->printDictionary(std::cerr);
        gene2.getKmerHandler()->printDictionary(std::cerr);
        #endif
        // errore
        kmers::KmersHandler::kmersSetTypePointer set1 = gene1.getKmerHandler()->getSet();
        kmers::KmersHandler::kmersSetTypePointer set2 = gene2.getKmerHandler()->getSet();
        
        #ifdef DEBUG
        std::cerr<<"\nsets created";
        std::cerr<<"\n";
        for (auto s = set1->begin(); s != set1->end(); ++s){
            std::cerr<<s->first<<", ";
        }
        std::cerr<<"\n";
        for (auto s = set2->begin(); s != set2->end(); ++s){
            std::cerr<<s->first<<", ";
        }
        
        #endif

        kmers::KmersHandler::KmersUnionTypePointer kmersUnion = makeUnion(set1, set2);
        
        #ifdef DEBUG
        std::cerr<<"\nunion done\n";
        for(auto str = kmersUnion->begin(); str != kmersUnion->end(); ++str){
            std::cerr<<*str<<", ";
        }
        #endif
        
        // num
        long num = 0;
        
        // den
        long den = 0;

        #ifdef DEBUG2
        std::cerr<<"\n---------------\n";
        #endif

        for(auto str = kmersUnion->begin(); str != kmersUnion->end(); ++str) {
            

            auto iter1 = set1->find((*str));
            auto iter2 = set2->find((*str));
            
            #ifdef DEBUG2
            std::cerr<<"\n iter1 "<<(iter1 == set1->end());
            std::cerr<<"\n iter2 "<<(iter2 == set2->end());
            #endif

            if(iter1 != set1->end() && iter2 != set2->end()){
                #ifdef DEBUG2
                std::cerr<<"\n iter1: "<<iter1->first;
                std::cerr<<"\n iter2: "<<iter2->first;
                #endif
                num += std::min(
                    iter1->second->getMultiplicity(),
                    iter2->second->getMultiplicity()
                );
                den += std::max(
                    iter1->second->getMultiplicity(),
                    iter2->second->getMultiplicity()
                );
            }else{
                if(iter1 == set1->end() && iter2 != set2->end())
                    den += iter2->second->getMultiplicity();
                else
                    den += iter1->second->getMultiplicity();
            }
        }
        #ifdef DEBUG
        std::cerr<<"\nden: "<<den<<"\nnum: "<<num;
        #endif
        

        double simil = 1.0*num/den;

        delete set1;
        delete set2;
        delete kmersUnion;
        return simil;
    }

    // 3 bidirectional similarity
    void HomologyDetector::calculateBidirectional(){

    }

    void HomologyDetector::searchParalogy(genomeTypeReference genome) {
        
    }


    
}


#endif