#ifndef ARMAMAGNA_H
#define ARMAMAGNA_H

#include <fstream>         //For std::ostream
#include <string>          //For std::string
#include <vector>          //For std::vector
#include <memory>          //For std::unique_ptr
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
    ArmaMagna();
    auto anagram() -> std::expected<unsigned long long, std::string>;
    auto setOptions(const std::string &text, const std::string &dictionary, const std::string &included, int mincard, int maxcard)
                    -> std::expected<void, std::string>;

    //Getters
    const std::string &getSourceText() const;
    const std::string &getDictionaryName() const;
    const std::string &getIncludedText() const;
    int getMinCardinality() const;
    int getMaxCardinality() const;
    int getThreadsNumber() const;

    //Setters
    auto setSourceText(const std::string targetText) -> std::expected<void, std::string>;
    void setDictionaryName(const std::string dictionaryName);
    auto setIncludedText(const std::string includedText) -> std::expected<void, std::string>;
    auto setRestrictions(int minCardinality, int maxCardinality) -> std::expected<void, std::string>;
    void setThreadsNumber();

private:
    //Constructor arguments
    std::string targetText;
    std::string dictionaryName;
    std::string includedText;
    int minCardinality, maxCardinality;

    //Processed variables
    std::unique_ptr<Dictionarium> dictionaryPtr;
    WordSignature targetSignature, includedTextSignature, actualTargetSignature; //actualTargetSignature = targetTextSignature - includedTextSignature
    int includedWordsNumber;
    int effectiveMinCardinality, effectiveMaxCardinality;
    unsigned int supportedConcurrency; //std::thread::hardware_concurrency();

    /***************SHARED RESOURCES***************/

    //Thread-safe output queue
    std::queue<std::string> anagramQueue;
    std::mutex anagramQueueMutex;
    std::condition_variable anagramQueueCV;

    /*********************************************/

    //I/O
    std::atomic<bool> searchIsComplete = false; //Flag to signal I/O thread to stop☢️
    std::ofstream outputFile;
    std::jthread ioThread;

    //Statistics
    unsigned long long anagramCount = 0;

    //Private functions
    void print();  //Debug print function
    void ioLoop();                //Thread that writes anagrams to file
};

#endif
