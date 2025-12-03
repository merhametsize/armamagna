#include <iostream>    //For std::cout
#include <string>      //For std::string

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

    CLI::App app("ArmaMagna");

    app.add_option("text", source, "Text to anagram")->required();
    app.add_option("-d,--dict", dictionary, "Dictionary file path")->required();
    app.add_option("-i,--incl", includedText, "Included text");
    app.add_option("--mincard", minCardinality, "Minimum cardinality")->required()->check(CLI::PositiveNumber);
    app.add_option("--maxcard", maxCardinality, "Maximum cardinality")->required()->check(CLI::PositiveNumber);
    app.footer("Example:\n\tarmamagna \"bazzecole andanti\" -d dizionario-italiano.txt --mincard=1 --maxcard=3\n\n" \
                "Author: Gabriele Cassetta, @merhametsize");

    CLI11_PARSE(app, argc, argv);

    //Starts anagramming
    ArmaMagna am(source, dictionary, includedText, minCardinality, maxCardinality);
    auto anagramsFound = am.anagram();
    if(!anagramsFound) {std::cout << anagramsFound.error() << std::endl; return -1;}

    return 0;
}