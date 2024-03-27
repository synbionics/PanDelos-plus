#include <iostream>
#include <unistd.h>

#include "lib/Homology.hh"
#include "lib/genx/GenomesContainer.hh"
#include "utils/FileLoader.hh"
#include "utils/StopWatch.hh"

using namespace homology;
using namespace genome;
using namespace utilities;


// ! todo: aggiungere la possibilità di scegliere il criterio di filtraggio
// ! todo: spostare analisi e calcoli vari in homology
// ! todo: commentare codice


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
*/
void parser(int argc, char *argv[], int& k, std::string& inFile, std::string& outFile, ushort& threadNum, bool& mode) {
    int option;
    while ((option = getopt(argc, argv, "i:o:k:t:hm")) != -1) {
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
            case 'm':
                mode = true;
                // if(atoi(optarg) != 0)
                break;
            case 'h':
                printTitle();
                #ifdef DEV_MODE
                std::cerr<<"Utilizzo:\n"
                        <<"-i per selezionare il file di input (path_to_file/file.faa)\n"
                        <<"-o per indicare il path/file di output\n"
                        <<"-k per indicare la dimensione dei kmers\n"
                        <<"-t per indicare il numero di thread\n"
                        <<"-m per attivare la modalità con un costo minore in ram (0 default)\n";
                #else
                std::cout<<"Usage:\n"
                    <<"-i to select the input file (path_to_file/file.faa)\n"
                    <<"-o to specify the file for output(path_to_file/file_name.extension)\n"
                    <<"-k to indicate the size of kmers\n"
                    <<"-t to indicate the number of threads\n"
                    <<"-m to activate specific mode with lower RAM cost (0 default)\n";
                #endif
                exit(1);

            case '?':
                printTitle();
                #ifdef DEV_MODE
                if (optopt == 'i' || optopt == 'o' || optopt == 'k' || optopt == 't')
                    std::cerr<<"L'opzione -"<<static_cast<char>(optopt)<<" richiede un argomento\n";
                else
                    std::cerr<<"Opzione sconosciuta: -"<<static_cast<char>(optopt)<<"\n";
            
                #else
                if (optopt == 'i' || optopt == 'o' || optopt == 'k' || optopt == 't')
                    std::cerr<<"Option -"<<static_cast<char>(optopt)<<" requires an argument\n";
                else
                    std::cerr<<"Unknown option: -"<<static_cast<char>(optopt)<<"\n";
                
                #endif
                exit(1);
            default:
                printTitle();
                #ifdef DEV_MODE
                std::cerr<<"Utilizzo:\n"
                        <<"-i per selezionare il file di input (path_to_file/file.faa\n"
                        <<"-o per indicare il path/file di output\n"
                        <<"-k per indicare la dimensione dei kmers\n"
                        <<"-t per indicare il numero di thread\n"
                        <<"-m per attivare la modalità con un costo minore in ram (0 default)\n";
                #else
                std::cout<<"Usage:\n"
                    <<"-i to select the input file (path_to_file/file.faa)\n"
                    <<"-o to specify the file for output(path_to_file/file_name.extension)\n"
                    <<"-k to indicate the size of kmers\n"
                    <<"-t to indicate the number of threads\n"
                    <<"-m to activate specific mode with lower RAM cost (0 default)\n";
                #endif
                exit(1);
        }
    }
}


int main(int argc, char *argv[]){

	int k = 0;
    ushort threadNum = 0;
    std::string inFile;
    std::string outFile;
    bool mode = false;

    parser(argc, argv, k, inFile, outFile, threadNum, mode);

    // use all threads
    GenomesContainer gh;
    FileLoader fl(inFile);
    fl.loadFile(gh);
    if(k == 0)
		exit(-1);
    if(threadNum == 0) {
        Homology hd(k, outFile);
        hd.calculateBidirectionalBestHit(gh, mode);
    } else {
        Homology hd(k, outFile, threadNum);
        hd.calculateBidirectionalBestHit(gh, mode);
    }
    std::cerr<<"\n";
    return 0;
}
