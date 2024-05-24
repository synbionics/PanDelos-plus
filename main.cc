#include <iostream>
#include <unistd.h>
#include <thread>

#include "lib/Homology.hh"
#include "lib/genx/GenomesContainer.hh"
#include "utils/FileLoader.hh"
#include "utils/StopWatch.hh"


using namespace homology;
using namespace genome;
using namespace utilities;


/**
 * @brief Prints the title.
 *
 * This function prints the title in ASCII art format.
 * The title consists of the string "ParPandelos" surrounded by ASCII art borders.
 * The output is sent to either std::cerr or std::cout based on the DEV_MODE macro.
 * 
 * If DEV_MODE is defined:
 * - The output is sent to std::cerr.
 * If DEV_MODE is not defined:
 * - The output is sent to std::cout.
*/
void printTitle() {
    #ifdef DEV_MODE
        std::cerr<<" +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+\n";
        std::cerr<<" |P| |a| |r| |P| |a| |n| |D| |e| |l| |o| |s|\n";
        std::cerr<<" +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+\n";
    #else
        std::cout<<" +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+\n";
        std::cout<<" |P| |a| |r| |P| |a| |n| |D| |e| |l| |o| |s|\n";
        std::cout<<" +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+\n";
    #endif
}
void printHelp() {
    #ifdef DEV_MODE
    std::cerr<<"Utilizzo:\n"
            <<"-i per selezionare il file di input (path_to_file/file.faa\n"
            <<"-o per indicare il path/file di output\n"
            <<"-k per indicare la dimensione dei kmers (1 default)\n"
            <<"-t per indicare il numero di thread\n"
            <<"-m per attivare la modalità con un costo minore in ram (0 default)\n"
            <<"-d per selezionare un valore di scarto (0 <= d <= 1) per il calcolo della similarità (0.5 default, un valore maggiore corrisponde a un scarto più aggressivo)\n";
    #else
    std::cout<<"Usage:\n"
        <<"-i to select the input file (path_to_file/file.faa)\n"
        <<"-o to specify the file for output(path_to_file/file_name.extension)\n"
        <<"-k to indicate the size of kmers\n"
        <<"-t to indicate the number of threads\n"
        <<"-m to activate specific mode with lower RAM cost (0 default)\n"
        <<"-d to select a discard value (0 <= d <= 1) for similarity computation (0.5 default, a grater value implies a more aggressive discard)\n";
    #endif
}
/**
 * @brief Parse command line arguments.
 *
 * This function parses command line arguments using getopt and sets the corresponding
 * variables according to the options provided.
 *
 * @param argc The number of command line arguments.
 * @param argv The array of command line arguments.
 * @param k Reference to an integer to store the dimension of kmers.
 * @param inFile Reference to a string to store the input file path.
 * @param outFile Reference to a string to store the output file path.
 * @param threadNum Reference to an unsigned short to store the number of threads.
 * @param mode Reference to a boolean to indicate a specific mode.
 * @param discard Reference to a float to indicate a discard value during similarity computation.
*/
void parser(int argc, char *argv[], int& k, std::string& inFile, std::string& outFile, ushort& threadNum, bool& mode, float& discard, bool& grid) {
    int option;
    while ((option = getopt(argc, argv, "d:i:o:k:t:hmg")) != -1) {
        switch (option) {
            case 'i':
                inFile = optarg;
                break;
            case 'o':
                outFile = optarg;
                break;
            case 'k':
                k = atoi(optarg);
                break;
            case 't':
                threadNum = atoi(optarg);
                break;
            case 'd':
                discard = atof(optarg);
                if(discard > 1 || discard < 0) {
                    printTitle();
                    printHelp();
                    exit(1);
                }
                break;
            case 'm':
                mode = true;
                break;
            case 'g':
                grid = true;
                break;
            case 'h':
                printTitle();
                printHelp();
                exit(1);
                break;
            case '?':
                printTitle();
                printHelp();
                exit(1);
                break;
            default:
                printTitle();
                printHelp();
                exit(1);
                break;
        }
    }
}


int main(int argc, char *argv[]){

	int k = 1;
    ushort threadNum = 0;
    std::string inFile = "";
    std::string outFile = "";
    bool mode = false;
    float discard = 0.5;
    bool grid = false;
    parser(argc, argv, k, inFile, outFile, threadNum, mode, discard, grid);

    #ifndef DEV_MODE
        std::cerr<<"\nDiscard value: "<<discard;
        std::cerr<<"\nInput File: "<<inFile;
        std::cerr<<"\nOutput File: "<<outFile;
        std::cerr<<"\nMode: "<<mode;
        std::cerr<<"\nThread number: "<<threadNum;
        std::cerr<<"\nK: "<<k;
        std::cerr<<"\nGrid: "<<grid;
    #else
        std::cout<<"\nDiscard value: "<<discard;
        std::cout<<"\nInput File: "<<inFile;
        std::cout<<"\nOutput File: "<<outFile;
        std::cout<<"\nMode: "<<mode;
        std::cout<<"\nThread number: "<<threadNum;
        std::cout<<"\nK: "<<k;
    #endif

    if(inFile == "" || outFile == "" || k == 0) {
        exit(1);
    }

    GenomesContainer gh;
    FileLoader fl(inFile);
    fl.loadFile(gh);

    std::cerr<<"\nStarting";
    try{
    if(threadNum == 0 || threadNum > std::thread::hardware_concurrency()) {
        Homology hd(k, outFile, discard);
        hd.calculateBidirectionalBestHit(gh, mode, grid);
    } else {
        Homology hd(k, outFile, threadNum, discard);
        hd.calculateBidirectionalBestHit(gh, mode, grid);
    }

    } catch (std::exception& e) {
        throw e;
    }
    std::cerr<<"\n";
    return 0;
}
