#include <string>
#include <iostream>
#include <cstddef>
#include <unordered_map>

#include "./Kmer.hh"

#ifndef KMER_HANDLER_INCLUDE_GUARD
#define KMER_HANDLER_INCLUDE_GUARD

// #define DEBUG 1
// #define DEBUG2 1

namespace kmers {
    class KmersHandler{
        private:
            using sequenceType = const std::string;
            using sequenceTypeReference = const sequenceType&;
            
            using kType = const unsigned short;
            
            using mapKeyType = std::size_t;

            using KmerType = Kmer;
            using KmerTypePointer = KmerType*;

            using kDictionaryType = std::unordered_map<mapKeyType, KmerTypePointer>;
            using kDictionaryTypePointer = kDictionaryType*;

            using kmersSetType = std::unordered_map<std::string, KmerTypePointer>;

            // reference perché così non bisogna deferenziare
            // il puntatore
            sequenceTypeReference alphabet_;
            kType k_;

            // Set di tutti i k-mers differenti all'interno della stringa
            // alphabet_
            // ! allocazione e deallocazione gestita in questa classe
            kDictionaryTypePointer dictionary_;


        public:

            KmersHandler() = delete;
            KmersHandler(kType k_length, sequenceTypeReference alphabet);
        
            ~KmersHandler();

            using KmersUnionType = std::vector<std::string>;
            using KmersUnionTypePointer = KmersUnionType*;
            using kmersSetTypePointer = kmersSetType*;

            
            std::string calculateKmersString() const;

            static KmersUnionTypePointer makeUnion(KmersHandler::kmersSetTypePointer set1, KmersHandler::kmersSetTypePointer set2);
            kmersSetTypePointer getSet() const;

            void calculateKmers();

            void printDictionary(std::ostream& os) const;
            // if present return multiplicity, else return 0
            unsigned short hasKmer(std::string& str) const;

    };
    
    KmersHandler::KmersHandler(kType k_length, sequenceTypeReference alphabet) : k_{k_length}, alphabet_{alphabet} {
        dictionary_ = new kDictionaryType();
        #ifdef DEBUG
            std::cerr<<"\nNew KmersHandler:\n";
            std::cerr<<"- k_: "<<k_<<"\n- alphabet_: "<<alphabet_<<"\n";
        #endif
    }
    
    KmersHandler::~KmersHandler() {
        for(auto it = dictionary_->begin(); it != dictionary_->end(); ++it){
            delete it->second;
        }
        delete dictionary_;
    }

    void KmersHandler::calculateKmers() {
        // per la memorizzazione dei vari Kmers si utilizza una
        // unordered_map con la coppia <first_index, Kmer> dove
        // first_index corrisponde all'indice del primo carattere
        // del primo Kmer all'interno della stringa.

        std::unordered_map<std::string, Kmer*>* currentKmers = new std::unordered_map<std::string, Kmer*>();
        
        mapKeyType kmersNum = alphabet_.length() - k_ + 1;

        #ifdef DEBUG
            std::cerr<<"\nkmersNum: "<<kmersNum<<"\n";
        #endif

        for(std::size_t i = 0; i < kmersNum; ++i){
            std::string ss = alphabet_.substr(i, k_);

            #ifdef DEBUG
                std::cerr<<"\ni: "<<i<<"\nss: "<<ss<<"\n";
            #endif

            auto iter = currentKmers->find(ss);

            if(iter == currentKmers->end()){
                #ifdef DEBUG
                std::cerr<<"\nadding missing\n";
                #endif
                Kmer* tmpKmer = new Kmer(i);
                currentKmers->insert(std::make_pair(ss, tmpKmer));

                dictionary_->insert(std::make_pair(i, tmpKmer));
                #ifdef DEBUG
                std::cerr<<"\nmissing added\n";
                #endif
            }else{
                #ifdef DEBUG
                std::cerr<<"\nAlready present\n";
                #endif
                iter->second->addIndex(i);
                // no
                // dictionary_->at((*iter).second->getFirstIndex())->addIndex(i);
            }
        }

        #ifdef DEBUG2
        printDictionary(std::cerr);
        #endif

        delete currentKmers;
    }
    
    // object returned is not deallocated
    KmersHandler::kmersSetTypePointer KmersHandler::getSet() const {
        kmersSetTypePointer retSet = new kmersSetType();
        for(auto kmer = dictionary_->begin(); kmer != dictionary_->end(); ++kmer) {
            retSet->insert(
                std::make_pair(
                    alphabet_.substr(kmer->first, k_),
                    kmer->second
                )
            );
        }
        return retSet;
    }

    

    static KmersHandler::KmersUnionTypePointer makeUnion(KmersHandler::kmersSetTypePointer set1, KmersHandler::kmersSetTypePointer set2) {
        KmersHandler::KmersUnionTypePointer kmersUnion = new KmersHandler::KmersUnionType();
        
        // I'll add kmer in the kmersUnion only if it is contained
        // in only one set
        for(auto kmer = set1->begin(); kmer != set1->end(); ++kmer) {
            auto res = set2->find(kmer->first);
            if(res == set2->end())
                kmersUnion->push_back(kmer->first);
            // else discard
        }
        // add all remaining k-mers
        for(auto kmer = set2->begin(); kmer != set2->end(); ++kmer)
            kmersUnion->push_back(kmer->first);

        return kmersUnion;
    }


    // testing function
    std::string KmersHandler::calculateKmersString() const{
        // per la memorizzazione dei vari Kmers si utilizza una
        // unordered_map con la coppia <first_index, Kmer> dove
        // first_index corrisponde all'indice del primo carattere
        // del primo Kmer all'interno della stringa.

        std::unordered_map<std::string, Kmer*>* currentKmers = new std::unordered_map<std::string, Kmer*>();
        
        mapKeyType kmersNum = alphabet_.length() - k_ + 1;

        #ifdef DEBUG
            std::cerr<<"\nkmersNum: "<<kmersNum<<"\n";
        #endif

        for(unsigned int i = 0; i < kmersNum; ++i){
            std::string ss = alphabet_.substr(i, k_);

            #ifdef DEBUG
                std::cerr<<"\ni: "<<i<<"\nss: "<<ss<<"\n";
            #endif

            auto iter = currentKmers->find(ss);

            if(iter == currentKmers->end()){
                #ifdef DEBUG
                std::cerr<<"\nadding missing\n";
                #endif
                currentKmers->insert(std::make_pair(ss, new Kmer(i)));
                #ifdef DEBUG
                std::cerr<<"\nmissing added\n";
                #endif
            }else{
                #ifdef DEBUG
                std::cerr<<"\nAlready present\n";
                #endif
                iter->second->addIndex(i);
            }
        }

        #ifdef DEBUG
        std::cerr<<"\nret\n";
        #endif
        std::string ret = "";

        for(auto it = currentKmers->begin(); it != currentKmers->end(); ){

            ret += it-> first + "(";
            ret += std::to_string(it->second->getMultiplicity());
            ret += ")";
            delete (*it).second;
            if(++it != currentKmers->end())
                ret += ", ";
        }
        
        delete currentKmers;
        
        return ret;

    }
    void KmersHandler::printDictionary(std::ostream& os) const{
        for(auto it = dictionary_->begin(); it != dictionary_->end(); ++it){

            os<<"\n";
            os<<it->first<<") {"<<alphabet_.substr(it->first, k_)<<"}:\n";
            it->second->print(os);
        }
    }
    unsigned short KmersHandler::hasKmer(std::string& str) const{
        for(auto kmer = dictionary_->begin(); kmer != dictionary_->end(); ++kmer){
            if(alphabet_.substr(kmer->first, k_) == str){
                return kmer->second->getMultiplicity();
            }
        }
        return 0;
    }

    
}


#endif