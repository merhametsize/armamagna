#include <unordered_map>  //For std::unorderd_map
#include <stdexcept>     //For std::invalid_argument and std::exception
#include <fstream>      //For std::ifstream
#include <cassert>     //For assert
#include <string>     //For std::string
#include <ranges>    //For std::views
#include <set>      //For std::set

#include "WordSignature.h"
#include "StringNormalizer.h"
#include "Dictionarium.h"

const int maxWordLength = 30; //If a word longer than 'maxWordLength' is found, program terminates

Dictionarium::Dictionarium(const std::string &dictionaryName, const std::string &sourceText)
{
    //Variable initialization
    wordsNumber = 0;
    effectiveWordsNumber = 0;
    longestWordLength = 0;

    //Initializes the sections, creating a map for each one
    sections.resize(maxWordLength+1); //sections[0] is not valid
    for(int i=0; i<maxWordLength+1; i++) sections.emplace_back();

    try
    {
        readDictionary(dictionaryName, sourceText);
    }
    catch(std::invalid_argument &e) {throw std::invalid_argument(e.what());}
}

void Dictionarium::readDictionary(const std::string &dictionaryName, const std::string &sourceText)
{
    //Opens the file
    std::ifstream file(dictionaryName, std::ios::in);
    if(!file) {std::string msg = "cannot open "; msg += dictionaryName; throw std::invalid_argument(msg);}

    //Computes the source text signature
    std::string normalizedSourceText;
    try {normalizedSourceText = StringNormalizer::normalize(sourceText);} //Normalizes the source text
    catch (std::invalid_argument &e) {throw std::invalid_argument(e.what());}
    WordSignature sourceSignature(normalizedSourceText);

    //Reads the dictionary line by line
    std::string word;
    while(getline(file, word))
    {
        //Normalizes the word
        std::string normalizedWord;
        try {normalizedWord = StringNormalizer::normalize(word);}
        catch (std::invalid_argument &e) {throw std::invalid_argument(e.what());}

        if(normalizedWord.empty()) continue; //Skip empty normalized words

        //If it's longer than maxWordLength, exception
        const size_t wordLength = normalizedWord.length();
        if(wordLength > maxWordLength)
        {
            std::string msg = "a word in the dictionary is too long (";
            msg += word; msg += "), maximum length is "; msg += std::to_string(maxWordLength); msg += " characters";
            throw std::invalid_argument(msg);
        }

        //Computes the word's signature
        WordSignature ws(normalizedWord);
        wordsNumber++;
        if(!ws.isSubsetOf(sourceSignature)) continue; //If the word is not a subset of the text to be anagrammed, skips it

        //Refreshes the length of the longest word
        if(normalizedWord.length() > longestWordLength) longestWordLength = normalizedWord.length();

        //Looks for the signature in the right section
        Section &rightSection = sections.at(static_cast<int>(wordLength));
        auto it = rightSection.find(ws);
        if(it == rightSection.end())   //If the signature is not yet in the map
        {
            std::vector<std::string> wordVector;               //Creates a new set of std::strings
            wordVector.emplace_back(word);                    //Puts the word in it
            rightSection.emplace(ws, std::move(wordVector)); //Creates a new map entry with the signature and the word

            effectiveWordsNumber++;
        }
        else //If the signature is already in the map
        {
            it->second.push_back(word); //Adds the word to the set

            effectiveWordsNumber++;
        }
    }
}

unsigned long Dictionarium::getWordsNumber() const
{
    return wordsNumber;
}

unsigned long Dictionarium::getEffectiveWordsNumber() const
{
    return effectiveWordsNumber;
}

size_t Dictionarium::getLongestWordLength() const
{
    return longestWordLength;
}

const Section &Dictionarium::getSection(int sectionNumber) const
{
    assert(sectionNumber > 0 && sectionNumber <= maxWordLength);
    return sections.at(sectionNumber);
}

const std::vector<std::string> &Dictionarium::getWords(const WordSignature &ws) const
{
    const int charactersNumber = ws.getCharactersNumber(); //Gets the section index
    return sections.at(charactersNumber).at(ws);          //Returns the set of words associated to ws
}

const std::vector<int> Dictionarium::getAvailableLengths() const 
{
    std::vector<int> availableLengths;
    for(auto [i, s]: sections | std::views::enumerate)
    {
        if(!s.empty()) availableLengths.push_back(static_cast<int>(i));
    }
    return availableLengths;
}

std::ostream &operator<<(std::ostream &os, const Dictionarium &dict)
{
    for(const auto &section : dict.sections) //For every section (i.e. word length)
    {
        for(const auto &entry : section)   //For every entry in the section
        {
            const WordSignature &ws = entry.first;     //Get the signature
            const std::vector<std::string> &words = entry.second;  //Get the set of words associated to that signature

            os << ws;                                    //Outputs the signature
            for(std::string word : words) os << " " << word; //Outputs the words
            os << std::endl;
        }
    }
    return os;
}
