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
    auto setOptions(const std::string &text, const std::string &dictionary, const std::string& outputFileName, 
                    const std::string &included, int mincard, int maxcard, int numThreads)
                    -> std::expected<void, std::string>;

    //Setters
    auto setTargetText(const std::string targetText) -> std::expected<void, std::string>;
    void setDictionaryName(const std::string dictionaryName);
    auto setIncludedText(const std::string includedText) -> std::expected<void, std::string>;
    auto setRestrictions(int minCardinality, int maxCardinality) -> std::expected<void, std::string>;
    void setThreadsNumber(unsigned int n);

private:
    //Constructor arguments
    std::string targetText;
    std::string includedText;
    std::string dictionaryName;
    std::string outputFileName;
    int minCardinality, maxCardinality;

    //Processed variables
    Dictionarium dictionary; //Shared but read-only for threads
    WordSignature targetSignature, includedTextSignature, actualTargetSignature; //actualTargetSignature = targetTextSignature - includedTextSignature
    int includedWordsNumber;
    int actualMinCardinality, actualMaxCardinality;
    unsigned int numThreads;

    /***************SHARED RESOURCES***************/

    //Thread-safe output queue
    std::queue<std::string> anagramQueue;
    std::mutex anagramQueueMutex;
    std::condition_variable anagramQueueCV;

    /*********************************************/

    //I/O
    std::atomic<bool> searchIsComplete = false; //Flag to signal I/O thread to stop☢️
    std::ofstream ofstream;
    std::jthread ioThread;

    //Statistics
    unsigned long long anagramCount = 0;
    size_t setsNumber = 0;                       //Number of repeated combinations with sum found
    std::atomic<size_t> exploredSetsNumber = 0; //Progress index, incremented at the end of each search thread☢️

    //Private functions
    void print();                               //Debug print function
    void ioLoop(); //Thread that writes anagrams to file
};

#endif
