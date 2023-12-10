#include <string>
#include <vector>
#include <cstddef>

#ifndef KMER_INCLUDE_GUARD
#define KMER_INCLUDE_GUARD

// #define DEBUG 1

namespace kmers {

    class Kmer{
        private:
            using multiplicityType = std::size_t;
            using indexType = std::size_t;
            
            //! Si potrebbe sostituire con una mappa o con un set? 
            //! Nel caso avrei accesso in O(1)?
            using indexTypeContainer = std::vector<indexType>;
            using indexTypeContainerPointer = indexTypeContainer*;
            //! Allaocazione e deallocazione gestita qui
            indexTypeContainerPointer begins_;


        public:
            Kmer(indexType index);
            void addIndex(indexType index);

            // quante volte un kmer si presenta in una stringa s
            multiplicityType getMultiplicity() const;

            indexType getFirstIndex() const;

            void print(std::ostream& os) const;
            bool operator==(const Kmer& other) const;
            ~Kmer();
    };
    
    Kmer::Kmer(indexType index) {
        
        begins_ = new indexTypeContainer();
        begins_->push_back(index);
        #ifdef DEBUG
        std::cerr<<"\nNew Kmer: \n- multiplicity: "<<multiplicity_;
        #endif
    }
    void Kmer::addIndex(indexType index){
        
        begins_->push_back(index);
        #ifdef DEBUG
        std::cerr<<"\nAdding index:\n - multiplicity: "<<multiplicity_;
        #endif
    }

    Kmer::multiplicityType Kmer::getMultiplicity() const{
        return this->begins_->size();
    }
    
    Kmer::indexType Kmer::getFirstIndex() const{
        return this->begins_->at(0);
    }

    void Kmer::print(std::ostream& os) const{

        os<<"begins:\n";
        for(auto it = begins_->begin(); it != begins_->end(); ++it){
            os<<(*it)<<", ";
        }
        os<<"\nMultiplicity: "<<this->begins_->size();
    }

    

    Kmer::~Kmer() {
        delete begins_;
    }

}


#endif