#include <iostream>    //For std::cout
#include <iomanip>     //For std::setw, std::left, std::right
#include <string>      //For std::string, std::stoi
#include <stdexcept>   //For std::invalid_argument
#include <thread>      //For std::thread::hardware_concurrency

#include "CommandLineParser.h"
#include "ArmaMagna.h"

#include <CLI11.hpp>

void printUsage();
bool readArguments(int argc, char **argv, std::string &source, std::string &dictionary, std::string &includedText,
                   int &minCardinality, int &maxCardinality);

int main(int argc, char **argv)
{
    //Command line argument variables, set to illegal value
    std::string source       = "";
    std::string dictionary   = "";
    std::string includedText = "";
    int minCardinality  = -1;
    int maxCardinality  = -1;

    //Reads command line arguments and checks their validity
    bool valid = readArguments(argc, argv, source, dictionary, includedText, minCardinality, maxCardinality);
    if(!valid) {printUsage(); return 0;}

    //Starts anagramming
    try
    {
        ArmaMagna am(source, dictionary, includedText, minCardinality, maxCardinality);
        am.anagram();
    }
    catch(std::invalid_argument &e) {std::cerr << "[x] Invalid argument: " << e.what() << std::endl;}
    catch(std::exception &e)        {std::cerr << "[x] Error: " << e.what() << std::endl;}

    return 0;
}

//Returns true if arguments are valid, false otherwise
bool readArguments(int argc, char **argv, std::string &source, std::string &dictionary, std::string &includedText,
                   int &minCardinality, int &maxCardinality)
{
    CLI::App app("ArmaMagna");

    app.add_option("text", source, "Text to anagram")->required();
    app.add_option("-d,--dict", dictionary, "Dictionary file path")->required();
    app.add_option("-i,--incl", includedText, "Included text");
    app.add_option("--mincard", minCardinality, "Minimum cardinality")->required();
    app.add_option("--maxcard", maxCardinality, "Maximum cardinality")->required();

    CLI11_PARSE(app, argc, argv);

    if(source.empty() || dictionary.empty()) return false;
    if(minCardinality < 0 || maxCardinality < 0) return false;

    return true;
}

void printUsage()
{
    std::cout << "Usage: armamagna <text> -d <dictionary> [options]\n";
    std::cout << "Multi-threaded anagrammer engine. Anagrams are found by combining words of a dictionary.\n";
    std::cout << "Accented and non-accented letters are considered the same letter (e.g. é è e).\n";
    std::cout << "Author: Gabriele Cassetta, @merhametsize, 2018.\n\n";
    std::cout << "Options:\n";
    std::cout << std::left << std::setw(10) << "-d <dict>" << std::setw(15) << "--dict=<dict>" << "[Mandatory] Specifies which dictionary shall be used\n";
    std::cout << std::left << std::setw(10) << "" << std::setw(15) << "--mincard=<n>" << "[Mandatory] Sets the minimum number of words in the anagrams\n";
    std::cout << std::left << std::setw(10) << "" << std::setw(15) << "--maxcard=<n>" << "[Mandatory] Sets the maximum number of words in the anagrams\n";
    std::cout << std::left << std::setw(10) << "-i <text>" << std::setw(15) << "--incl=<text>" << "Find anagrams that contain this text (default: none)\n";
    std::cout << "Examples:\n";
    std::cout << "\tarmamagna \"uncle pear\" -d english-dictionary.txt -i \"luna\" --mincard=1 --maxcard=3 --minwlen=1 --maxwlen=30\n";
    std::cout << "\tarmamagna \"sò cel a lo dësmentia mai\" -d dissionari-piemonteis.txt --mincard=2 --maxcard=5 --minwlen=2 --maxwlen=30";
    std::cout << std::endl;
}
