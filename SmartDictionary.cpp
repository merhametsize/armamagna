#include <iostream>
#include <fstream>    //For std::ifstream
#include <stdexcept>  //For std::invalid_argument and std::exception
#include <string>     //For std::string
#include <map>        //For std::map
#include <set>        //For std::set

#include "WordSignature.h"
#include "StringNormalizer.h"
#include "SmartDictionary.h"

using namespace std;

const int maxWordLength = 30; //If a word longer than 'maxWordLength' is found, program terminates

SmartDictionary::SmartDictionary(const string &dictionaryName, const string &sourceText)
{
    //Variable initialization
    wordsNumber = 0;
    effectiveWordsNumber = 0;
    longestWordLength = 0;

    //Initializes the sections, creating a map for each one
    sections.reserve(maxWordLength + 1);
    for(int i=1; i<=maxWordLength; i++) sections.push_back(map<WordSignature, set<string>>());

    try
    {
        readDictionary(dictionaryName, sourceText);
    }
    catch(invalid_argument &e) {throw invalid_argument(e.what());}
}

void SmartDictionary::readDictionary(const string &dictionaryName, const string &sourceText)
{
    //Opens the file
    ifstream file(dictionaryName, ios::in);
    if(!file) {string msg = "cannot open "; msg += dictionaryName; throw invalid_argument(msg);}

    //Computes the source text signature
    string normalizedSourceText;
    try {normalizedSourceText = StringNormalizer::normalize(sourceText);} //Normalizes the source text
    catch (invalid_argument &e) {throw invalid_argument(e.what());}
    WordSignature sourceSignature(normalizedSourceText);

    //Reads the dictionary line by line
    string word;
    while(getline(file, word))
    {
        //Normalizes the word
        string normalizedWord;
        try {normalizedWord = StringNormalizer::normalize(word);}
        catch (invalid_argument &e) {throw invalid_argument(e.what());}

        //If it's longer than maxWordLength, exception
        const size_t wordLength = normalizedWord.length();
        if(wordLength > maxWordLength)
        {
            string msg = "a word in the dictionary is too long (";
            msg += word; msg += "), maximum length is "; msg += maxWordLength; msg += " characters";
            throw invalid_argument(msg);
        }

        //Computes the word's signature
        WordSignature ws(normalizedWord);
        wordsNumber++;
        if(!ws.isSubsetOf(sourceSignature)) continue; //If the word is not a subset of the text to be anagrammed, skips it

        //Refreshes the length of the longest word
        if(normalizedWord.length() > longestWordLength) longestWordLength = normalizedWord.length();

        //Looks for the signature in the right section
        Section &rightSection = sections.at(wordLength);
        auto it = rightSection.find(ws);
        if(it == rightSection.end())   //If the signature is not yet in the map
        {
            set<string> wordSet;                   //Creates a new set of strings
            wordSet.emplace(word);                //Puts the word in it
            rightSection.emplace(ws, wordSet);   //Creates a new map entry with the signature and the word

            effectiveWordsNumber++;
        }
        else //If the signature is already in the map
        {
            it->second.emplace(word); //Adds the word to the set

            effectiveWordsNumber++;
        }
    }
}

unsigned long SmartDictionary::getWordsNumber() const
{
    return wordsNumber;
}

unsigned long SmartDictionary::getEffectiveWordsNumber() const
{
    return effectiveWordsNumber;
}

size_t SmartDictionary::getLongestWordLength() const
{
    return longestWordLength;
}

const Section &SmartDictionary::getSection(int sectionNumber) const
{
    return sections.at(sectionNumber);
}

const set<string> &SmartDictionary::getWords(const WordSignature &ws) const
{
    const int charactersNumber = ws.getCharactersNumber(); //Gets the section index
    return sections.at(charactersNumber).at(ws);          //Returns the set of words associated to ws
}

ostream &operator<<(ostream &os, const SmartDictionary &sd)
{
    for(const auto &section : sd.sections) //For every section (i.e. word length)
    {
        for(const auto &entry : section)   //For every entry in the section
        {
            const WordSignature &ws = entry.first;     //Get the signature
            const set<string> &words = entry.second;  //Get the set of words associated to that signature

            cout << ws;                                    //Outputs the signature
            for(string word : words) cout << " " << word; //Outputs the words
            cout << endl;
        }
    }
    return os;
}
