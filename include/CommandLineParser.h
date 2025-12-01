#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string> //For std::string
#include <vector> //For std::vector

class CommandLineParser
{
public:
    CommandLineParser(int argc, char **argv);

    //Getters
    std::string getOption(const std::string &longOption);
    std::string getOption(const std::string &shortOption, const std::string &longOption);
    int getOptionAsInt(const std::string &longOption);
    int getOptionAsInt(const std::string &shortOption, const std::string &longOption);
    
    std::string getArgument(int index);
    std::string getFirstArgument();
    std::string getLastArgument();

    bool isPresent(const std::string &longOption);
    bool isPresent(const std::string &shortOption, const std::string &longOption);
private:
    std::vector<std::string> tokens;
};

#endif
