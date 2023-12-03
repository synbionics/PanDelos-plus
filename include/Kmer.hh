#include <string>
#include <vector>
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

            
            // quante volte un kmer si presenta in una stringa s
            multiplicityType multiplicity_;



        public:
            Kmer(indexType index);
            void addIndex(indexType index);
            multiplicityType getMultiplicity() const;
            indexType getFirstIndex() const;

            void print(std::ostream& os) const;

            ~Kmer();
    };
    
    Kmer::Kmer(indexType index) : multiplicity_{1} {
        
        begins_ = new indexTypeContainer();
        begins_->push_back(index);
        #ifdef DEBUG
        std::cerr<<"\nNew Kmer: \n- multiplicity: "<<multiplicity_;
        #endif
    }
    void Kmer::addIndex(indexType index){
        
        ++multiplicity_;
        begins_->push_back(index);
        #ifdef DEBUG
        std::cerr<<"\nAdding index:\n - multiplicity: "<<multiplicity_;
        #endif
    }

    Kmer::multiplicityType Kmer::getMultiplicity() const{
        return this->multiplicity_;
    }
    
    Kmer::indexType Kmer::getFirstIndex() const{
        return this->begins_->at(0);
    }

    void Kmer::print(std::ostream& os) const{

        std::cerr<<"begins:\n";
        for(auto it = begins_->begin(); it != begins_->end(); ++it){
            std::cerr<<(*it)<<", ";
        }
        std::cerr<<"\nMultiplicity: "<<multiplicity_;
    }



    Kmer::~Kmer() {
        delete begins_;
    }
    
    
}


#endif