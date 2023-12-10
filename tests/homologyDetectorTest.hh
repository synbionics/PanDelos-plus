#include "./../include/Gene.hh"
#include "./../include/HomologyDetector.hh"
#include <assert.h>
namespace homology_tests {
    void test_00() {
        std::string sequence = "GTAGAGCTGT";
        gene::Gene g1(0, sequence);
        gene::Gene g2(1, sequence);
        homology_detector::HomologyDetector hd(3);
        double res = hd.calculateSimilarity(g1, g2);
        std::cerr<<"\nres = "<<res;
        assert(res == 1);
    }
    void test_01() {
        std::string sequence = "GTAGAGGTGT";
        std::string sequence2 = "GTGGAGGTGT";
        
        gene::Gene g1(0, sequence);
        gene::Gene g2(1, sequence2);

        homology_detector::HomologyDetector hd(3);
        double res = hd.calculateSimilarity(g1, g2);
        std::cerr<<"\nres = "<<res;
    }
    




    void launch() {
        test_00();
        test_01();
    }
}
