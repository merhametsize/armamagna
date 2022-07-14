#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <iostream>  //For std::ostream
#include <string>    //For std::string
#include <vector>    //For std::vector
#include <map>       //For std::map
#include <set>       //For std::set

#include "WordSignature.h"

//Every section of the dictionary is a map that associates a signature with 1 or more words
//Section[i] only contains words of length i
typedef std::map<WordSignature, std::set<std::string>> Section;

class SmartDictionary
{
    //Output debug function
    friend std::ostream &operator<<(std::ostream &os, const SmartDictionary &sd);

public:
    //The constructor needs the source text in order to exclude all words that are not a subset of it
    SmartDictionary(const std::string &dictionaryName, const std::string &sourceText);

    //Getters
    unsigned long getWordsNumber() const;                  //Returns the number of lines in the file
    unsigned long getEffectiveWordsNumber() const;        //Returns the number of non-excluded words in the dictionary
    size_t getLongestWordLength() const;                 //Returns the length of its longest word. Needed when creating the powerset
    const Section &getSection(int sectionNumber) const; //Returns a dictionary section. Needed in the search algorithm to iterate through signtures
    const std::set<std::string> &getWords(const WordSignature &ws) const; //Returns the set of words associated to a signature

private:
    unsigned int wordsNumber;
    unsigned int effectiveWordsNumber;
    size_t longestWordLength;
    std::vector<Section> sections;

    void readDictionary(const std::string &dictionaryName, const std::string &sourceText);
};

#endif
