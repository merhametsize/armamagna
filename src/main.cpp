#include <iostream>    //For std::cout
#include <iomanip>     //For std::setw, std::left, std::right
#include <string>      //For std::string, std::stoi
#include <stdexcept>   //For std::invalid_argument
#include <thread>      //For std::thread::hardware_concurrency

#include "CommandLineParser.h"
#include "ArmaMagna.h"

using namespace std;

void printUsage();
bool readArguments(int argc, char **argv, string &source, string &dictionary, string &includedText,
                   int &minCardinality, int &maxCardinality, int &minWordLength, int &maxWordLength, int &threads);

int main(int argc, char **argv)
{
    //Command line argument variables, set to illegal value
    string source       = "";
    string dictionary   = "";
    string includedText = "";
    int minCardinality  = -1;
    int maxCardinality  = -1;
    int minWordLength   = -1;
    int maxWordLength   = -1;
    int threads         = -1;

    //Reads command line arguments and checks their validity
    bool valid = readArguments(argc, argv, source, dictionary, includedText, minCardinality, maxCardinality, minWordLength, maxWordLength, threads);
    if(!valid) {printUsage(); return 0;}

    //Starts anagramming
    try
    {
        ArmaMagna am(source, dictionary, includedText, minCardinality, maxCardinality, minWordLength, maxWordLength, threads);
        am.anagram();
    }
    catch(invalid_argument &e) {cerr << "[x] Invalid argument: " << e.what() << endl;}
    catch(exception &e)        {cerr << "[x] Error: " << e.what() << endl;}

    return 0;
}

//Returns true if arguments are valid, false otherwise
bool readArguments(int argc, char **argv, string &source, string &dictionary, string &includedText,
                   int &minCardinality, int &maxCardinality, int &minWordLength, int &maxWordLength, int &threads)
{
    //Uses class CommandLineParser to parse command line arguments and fetch options
    CommandLineParser clp(argc, argv);

    //Help options, displays help and quits
    if(clp.isPresent("--help")) {printUsage(); exit(0);}

    //String options
    source       = clp.getFirstArgument();
    dictionary   = clp.getOption("-d", "--dict");
    includedText = clp.getOption("-i", "--incl");

    //Integer options, std::strtoi used in CommandLineParser might throw invalid_argument exception
    try
    {
        minCardinality = clp.getOptionAsInt("--mincard");
        maxCardinality = clp.getOptionAsInt("--maxcard");
        minWordLength  = clp.getOptionAsInt("--minwlen");
        maxWordLength  = clp.getOptionAsInt("--maxwlen");
        threads        = clp.getOptionAsInt("--threads");
    }
    catch (invalid_argument &e) {/*empty, variables in main will keep their illegal value*/}

    //Checks mandatory arguments' validity
    if(source == "" || dictionary == "") return false;
    if(minCardinality == -1 || maxCardinality == -1 || minWordLength == -1 || maxWordLength == -1) return false;

    //If the number of threads is not specified
    if(threads == -1)
    {
        threads = thread::hardware_concurrency(); //Finds out the number of cpu cores
        if(threads == 0) threads = 1;
    }

    return true;
}

void printUsage()
{
    cout << "Usage: armamagna <text> -d <dictionary> [options]\n";
    cout << "Multi-threaded anagrammer engine. Anagrams are found by combining words of a dictionary.\n";
    cout << "Accented and non-accented letters are considered the same letter (e.g. é è e).\n";
    cout << "Author: Gabriele Cassetta, @merhametsize, 2018.\n\n";
    cout << "Options:\n";
    cout << left << setw(10) << "-d <dict>" << setw(15) << "--dict=<dict>" << "[Mandatory] Specifies which dictionary shall be used\n";
    cout << left << setw(10) << "" << setw(15) << "--mincard=<n>" << "[Mandatory] Sets the minimum number of words in the anagrams\n";
    cout << left << setw(10) << "" << setw(15) << "--maxcard=<n>" << "[Mandatory] Sets the maximum number of words in the anagrams\n";
    cout << left << setw(10) << "" << setw(15) << "--minwlen=<n>" << "[Mandatory] Sets the minimum length of the words in the anagrams\n";
    cout << left << setw(10) << "" << setw(15) << "--maxwlen=<n>" << "[Mandatory] Sets the maximum length of the words in the anagrams\n";
    cout << left << setw(10) << "-i <text>" << setw(15) << "--incl=<text>" << "Find anagrams that contain this text (default: none)\n";
    cout << left << setw(10) << "" << setw(15) << "--threads=<n>" << "Sets the number of concurrent search threads (default: # of cpu cores)\n\n";
    cout << "Examples:\n";
    cout << "\tarmamagna \"uncle pear\" -d english-dictionary.txt -i \"luna\" --mincard=1 --maxcard=3 --minwlen=1 --maxwlen=30\n";
    cout << "\tarmamagna \"sò cel a lo dësmentia mai\" -d dissionari-piemonteis.txt --mincard=2 --maxcard=5 --minwlen=2 --maxwlen=30";
    cout << endl;
}
