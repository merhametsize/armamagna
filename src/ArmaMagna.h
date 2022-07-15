#ifndef ARMAMAGNA_H
#define ARMAMAGNA_H

#include <string>          //For std::string
#include <vector>          //For std::vector
#include <set>             //For std::set and std::multiset

#include "WordSignature.h"
#include "SmartDictionary.h"

class ArmaMagna
{
    friend class SearchThread;

public:
    ArmaMagna(const std::string &sourceText, const std::string &dictionaryName, const std::string &includedText,
                        int minCardinality, int maxCardinality, int minWordLength, int maxWordLength);
    void anagram();

    //Getters
    const std::string &getSourceText() const;
    const std::string &getDictionaryName() const;
    const std::string &getIncludedText() const;
    int getMinCardinality() const;
    int getMaxCardinality() const;
    int getMinWordLength() const;
    int getMaxWordLength() const;
    int getThreadsNumber() const;

    //Setters
    void setSourceText(const std::string &sourceText);
    void setDictionaryName(const std::string &dictionaryName);
    void setIncludedText(const std::string &includedText);
    void setRestrictions(int minCardinality, int maxCardinality, int minWordLength, int maxWordLength);
    void setThreadsNumber();

private:
    //Constructor arguments
    std::string sourceText;
    std::string dictionaryName;
    std::string includedText;
    int minCardinality, maxCardinality;
    int minWordLength, maxWordLength;

    //Processed variables
    SmartDictionary *dictionaryPtr;
    WordSignature sourceTextSignature, includedTextSignature, targetSignature; //targetSignature = sourceTextSignature - includedTextSignature
    int includedWordsNumber;
    int effectiveMinCardinality, effectiveMaxCardinality;
    unsigned int concurrentThreadsSupported; //Number of concurrent threads supported (esteem)

    //Set of anagrams, used to remove duplicates ("a b c" & "b c a" are the same anagram)
    std::set<std::multiset<std::string>> anagramSet;

    //Debug output function
    void print(std::ostream &os);
};

#endif
