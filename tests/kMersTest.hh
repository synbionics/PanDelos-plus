#include "../include/KmersHandler.hh"
#include <string>
#include <assert.h>

namespace kmers_tests{
    using namespace kmers;
    void test_00 (){
        unsigned int length = 1;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    
    void test_01 (){
        unsigned int length = 2;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_02 (){
        unsigned int length = 3;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_03 (){
        unsigned int length = 4;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_04 (){
        unsigned int length = 5;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_05 (){
        unsigned int length = 6;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_06 (){
        unsigned int length = 7;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_07 (){
        unsigned int length = 8;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_08 (){
        unsigned int length = 9;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    void test_09 (){
        unsigned int length = 10;
        std::string sequence = "GTAGAGCTGT";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }

    void test_10 (){
        unsigned int length = 3;
        std::string sequence = "MRVLKFGGTSVANAERFLRVADILESNARQGQVATVLSAPAKITNHLVAMIEKTISGQDALPNISDAERIFAELLTGLAAAQPGFPLAQLKTFVDQEFAQIKHVLHGISLLGQCPDSINAALICRGEKMSIAIMAGVLEARGHNVTVIDPVEKLLAVGHYLESTVDIAESTRRIAASRIPADHMVLMAGFTAGNEKGELVVLGRNGSDYSAAVLAACLRADCCEIWTDVDGVYTCDPRQVPDARLLKSMSYQEAMELSYFGAKVLHPRTITPIAQFQIPCLIKNTGNPQAPGTLIGASRDEDELPVKGISNLNNMAMFSVSGPGMKGMVGMAARVFAAMSRARISVVLITQSSSEYSISFCVPQSDCVRAERAMQEEFYLELKEGLLEPLAVTERLAIISVVGDGMRTLRGISAKFFAALARANINIVAIAQGSSERSISVVVNNDDATTGVRVTHQMLFNTDQVIEVFVIGVGGVGGALLEQLKRQQSWLKNKHIDLRVCGVANSKALLTNVHGLNLENWQEELAQAKEPFNLGRLIRLVKEYHLLNPVIVDCTSSQAVADQYADFLREGFHVVTPNKKANTSSMDYYHQLRYAAEKSRRKFLYDTNVGAGLPVIENLQNLLNAGDELMKFSGILSGSLSYIFGKLDEGMSFSEATTLAREMGYTEPDPRDDLSGMDVARKLLILARETGRELELADIEIEPVLPAEFNAEGDVAAFMANLSQLDDLFAARVAKARDEGKVLRYVGNIDEDGVCRVKIAEVDGNDPLFKVKNGENALAFYSHYYQPLPLVLRGYGAGNDVTAAGVFADLLRTLSWKLGV";
        KmersHandler kmersHandler(length, sequence);
        std::string out = kmersHandler.calculateKmersString();
        std::cerr<<"\n\n-------- sequence: "<<sequence<<" --------";     
        std::cerr<<"\nout: "<<out;
        kmersHandler.calculateKmers();
        kmersHandler.printDictionary(std::cerr);
    }
    

    void launch() {
        test_00();
        test_01();
        test_02();
        test_03();
        // test_04();
        // test_05();
        // test_06();
        // test_07();
        // test_08();
        // test_09();
        // test_10();
    }
}