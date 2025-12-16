#include <expected>      //For std::expected, std::unexpected
#include <fstream>      //For std::ifstream
#include <cassert>     //For assert
#include <string>     //For std::string
#include <ranges>    //For std::views
#include <set>      //For std::set

#include "ankerl/unordered_dense.h"
#include "WordSignature.h"
#include "StringNormalizer.h"
#include "Dictionarium.h"

Dictionarium::Dictionarium()
    : wordsNumber(0), actualWordsNumber(0), longestWordLength(0)
{
    //Empty
}

auto Dictionarium::readWordList(const std::string& dictionaryName, const std::string& sourceText) -> std::expected<int, std::string>
{
    //Opens the file
    std::ifstream file(dictionaryName, std::ios::in);
    if(!file) {return std::unexpected("Cannot open file " + dictionaryName);}

    //Computes the source text signature
    auto normalizedSourceText = StringNormalizer::normalize(sourceText); //Normalizes the source text
    if(!normalizedSourceText) {return std::unexpected(normalizedSourceText.error());}
    WordSignature sourceSignature(normalizedSourceText.value());

    //Reads the dictionary line by line
    std::string word;
    while(getline(file, word))
    {
        //Normalizes the word
        auto normalizedWord = StringNormalizer::normalize(word);
        if(!normalizedWord) {return std::unexpected(normalizedWord.error());}

        if(normalizedWord.value().empty()) continue; //Skip empty normalized words

        //If it's longer than maxWordLength, exception
        const size_t wordLength = normalizedWord.value().length();
        if(wordLength > MAX_WORD_LENGTH) 
            return std::unexpected("A word in the dictionary is too long, maximum length: " + std::to_string(MAX_WORD_LENGTH));

        //Computes the word's signature
        WordSignature ws(normalizedWord.value());
        wordsNumber++;
        if(!ws.isSubsetOf(sourceSignature)) continue; //If the word is not a subset of the text to be anagrammed, skips it
        actualWordsNumber++;

        //Refreshes the length of the longest word
        if(wordLength > longestWordLength) longestWordLength = wordLength;

        //Pushes the word in the right section, with the corresponding signature-key
        Section& rightSection = sections.at(wordLength);
        rightSection[ws].push_back(word);
    }

    return wordsNumber;
}

unsigned long Dictionarium::getWordsNumber() const
{
    return wordsNumber;
}

unsigned long Dictionarium::getActualWordsNumber() const
{
    return actualWordsNumber;
}

size_t Dictionarium::getLongestWordLength() const
{
    return longestWordLength;
}

const Section& Dictionarium::getSection(int sectionNumber) const
{
    return sections[sectionNumber]; //[] does NOT perform bounds checking, good for performance
}

const std::vector<std::string>& Dictionarium::getWords(const WordSignature& ws) const
{
    const int charactersNumber = ws.getCharactersNumber(); //Gets the section index
    return sections[charactersNumber].at(ws);             //[] does NOT perform bounds checking, good for performance
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

std::ostream& operator<<(std::ostream& os, const Dictionarium& dict)
{
    for(const auto& section : dict.sections) //For every section (i.e. word length)
    {
        for(const auto& entry : section)   //For every entry in the section
        {
            const WordSignature& ws = entry.first;     //Get the signature
            const std::vector<std::string>& words = entry.second;  //Get the set of words associated to that signature

            os << ws;                                    //Outputs the signature
            for(const std::string& word : words) os << " " << word; //Outputs the words
            os << std::endl;
        }
    }
    return os;
}
