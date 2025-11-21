#ifndef ARMAMAGNA_H
#define ARMAMAGNA_H

#include <fstream>         //For std::ostream
#include <string>          //For std::string
#include <vector>          //For std::vector
#include <memory>          //For std::unique_pointer
#include <set>             //For std::set and std::multiset

//Thread safety
#include <condition_variable>  //For std::condition_variable
#include <thread>              //For std::jthread
#include <atomic>              //For nuclear energy☢️
#include <mutex>               //For std::mutex
#include <queue>               //For std::queue

#include "WordSignature.h"
#include "Dictionarium.h"

class ArmaMagna
{
    friend class SearchThread;

public:
    ArmaMagna(const std::string &sourceText, const std::string &dictionaryName, const std::string &includedText,
                        int minCardinality, int maxCardinality);
    void anagram();

    //Getters
    const std::string &getSourceText() const;
    const std::string &getDictionaryName() const;
    const std::string &getIncludedText() const;
    int getMinCardinality() const;
    int getMaxCardinality() const;
    int getThreadsNumber() const;

    //Setters
    void setSourceText(const std::string &sourceText);
    void setDictionaryName(const std::string &dictionaryName);
    void setIncludedText(const std::string &includedText);
    void setRestrictions(int minCardinality, int maxCardinality);
    void setThreadsNumber();

private:
    //Constructor arguments
    std::string sourceText;
    std::string dictionaryName;
    std::string includedText;
    int minCardinality, maxCardinality;

    //Processed variables
    std::unique_ptr<Dictionarium> dictionaryPtr;
    WordSignature sourceTextSignature, includedTextSignature, targetSignature; //targetSignature = sourceTextSignature - includedTextSignature
    int includedWordsNumber;
    int effectiveMinCardinality, effectiveMaxCardinality;
    unsigned int concurrentThreadsSupported; //std::hread::hardware_concurrency();

    /***************SHARED RESOURCES***************/

    //Set of anagrams, used to remove duplicates ("a b c" & "b c a" are the same anagram)
    std::set<std::multiset<std::string>> anagramSet;
    std::mutex anagramSetMutex;

    //Thread-safe output queue
    std::queue<std::string> anagramQueue;
    std::mutex anagramQueueMutex;
    std::condition_variable anagramQueueCV;

    /*********************************************/

    //I/O
    std::atomic<bool> searchIsComplete = false; //Flag to signal I/O thread to stop☢️
    std::ofstream outputFile;
    std::jthread ioThread;

    //Private functions
    void print(std::ostream &os);  //Debug print function
    void ioLoop();                //Thread that writes anagrams to file
};

#endif
