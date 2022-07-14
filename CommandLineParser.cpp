#include <iostream>             //For I/O
#include <string>               //For std::string and std::stoi
#include <algorithm>            //For std::find
#include "CommandLineParser.h"

using namespace std;

CommandLineParser::CommandLineParser(int argc, char **argv)
{
    for(int i=0; i<argc; i++) tokens.push_back(argv[i]); //Adds every argument to the 'tokens' vector
}

string CommandLineParser::getOption(const string &shortOption, const string &longOption)
{
    size_t longOptionLength  = longOption.length();

    //Looks for the short option
    auto it = find(tokens.begin(), tokens.end(), shortOption);
    if(it != tokens.end() && (++it) != tokens.end()) //If it's found and there's a next argument
    {
        string nextArg = *it;                             //Gets the next argument
        if(nextArg.substr(0, 1) != "-") return nextArg;  //If it's not another option, returns it
    }

    //Looks for the long option
    for(const string &arg : tokens)
    {
        if(arg.length() > (longOptionLength + 1) && arg.substr(0, longOptionLength) == longOption)  //If it's found
            if(arg.substr(longOptionLength, 1) == "=")                                             //If there's an '='
                return arg.substr(longOptionLength + 1);                                          //Returns the value
    }

    return ""; //Otherwuse return an empty string
}

string CommandLineParser::getOption(const string &longOption)
{
    return getOption("", longOption);
}

int CommandLineParser::getOptionAsInt(const std::string &shortOption, const std::string &longOption)
{
    int result;
    string stringOption = getOption(shortOption, longOption);

    try
    {
        result = stoi(stringOption); //Converts the string to an integer
    }
    catch (invalid_argument &e) {throw invalid_argument(e.what());}

    return result;
}

int CommandLineParser::getOptionAsInt(const std::string &longOption)
{
    return getOptionAsInt("", longOption);
}

bool CommandLineParser::isPresent(const string &shortOption, const string &longOption)
{
    auto it = find(tokens.begin(), tokens.end(), shortOption);
    if(it != tokens.end()) return true;

    it = find(tokens.begin(), tokens.end(), longOption);
    if(it != tokens.end()) return true;

    return false;
}

bool CommandLineParser::isPresent(const string &longOption)
{
    return isPresent("", longOption);
}

string CommandLineParser::getArgument(int index)
{
    if(index < 0 || index >= static_cast<int>(tokens.size())) return "";
    return tokens.at(index);
}

string CommandLineParser::getFirstArgument()
{
    return getArgument(1);
}

string CommandLineParser::getLastArgument()
{
    return getArgument(tokens.size() - 1);
}
