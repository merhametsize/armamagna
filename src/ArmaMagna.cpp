#include <iostream>  //For I/O
#include <memory>    //For std::make_unique, std::unique_ptr
#include <string>    //For std::string
#include <vector>    //For std::vector
#include <chrono>    //For std::chrono
#include <ranges>    //For std::views, std::ranges::distance
#include <format>    //For std::format
#include <print>     //For std::print

#include "WordSignature.h"
#include "Dictionarium.h"
#include "StringNormalizer.h"
#include "Combinations.h"
#include "SearchThread.h"
#include "ArmaMagna.h"

#include <boost/asio.hpp>

int countWords(const std::string &str); //Counts words in a string

//Constructor
ArmaMagna::ArmaMagna(const std::string &text, const std::string &dictionary, const std::string &included, int mincard, int maxcard)
{
    try
    {
        setSourceText(text);
        setDictionaryName(dictionary);
        setIncludedText(included);
        setRestrictions(mincard, maxcard);
        setThreadsNumber();
    }
    catch(std::invalid_argument &e) {throw std::invalid_argument(e.what());}
}

//Getters
const std::string &ArmaMagna::getSourceText()     const {return sourceText;}
const std::string &ArmaMagna::getDictionaryName() const {return dictionaryName;}
const std::string &ArmaMagna::getIncludedText()   const {return includedText;}
int ArmaMagna::getMinCardinality()                const {return minCardinality;}
int ArmaMagna::getMaxCardinality()                const {return maxCardinality;}

//Setters
void ArmaMagna::setSourceText(const std::string text)
{
    this->sourceText = text;

    //Processes the source text and computes its signature
    std::string processedSourceText = StringNormalizer::normalize(text);
    sourceTextSignature = WordSignature(processedSourceText);
}

void ArmaMagna::setDictionaryName(const std::string dictionary)
{
    this->dictionaryName = dictionary;
}

void ArmaMagna::setIncludedText(const std::string included)
{
    this->includedText = included;

    //Processes the included text
    std::string processedIncludedText = StringNormalizer::normalize(included);
    includedTextSignature = WordSignature(processedIncludedText);

    //Computes the number of included words
    if(included == "") includedWordsNumber = 0;
    else               includedWordsNumber = countWords(included);

    //Invalid argument checking
    if(!includedTextSignature.isSubsetOf(sourceTextSignature)) throw std::invalid_argument("the included text must be a subset of the source text");
    if(sourceTextSignature == includedTextSignature)           throw std::invalid_argument("the included text is already an anagram of the source text");

    //target = source - included
    targetSignature = sourceTextSignature;
    targetSignature -= includedTextSignature;

    //Computes the effective cardinalities
    effectiveMinCardinality = minCardinality - includedWordsNumber;
    effectiveMaxCardinality = maxCardinality - includedWordsNumber;
}

void ArmaMagna::setRestrictions(int mincard, int maxcard)
{
    this->minCardinality = mincard;
    this->maxCardinality = maxcard;

    //Arguments validity checking
    if(mincard <= 0 || maxcard <= 0)      throw std::invalid_argument("cardinalities must be positive");
    if(mincard > maxcard)                 throw std::invalid_argument("maximum cardinality must be greater or equal to minimum cardinality");
    if(mincard <= includedWordsNumber)    throw std::invalid_argument("minimum cardinality must be greater than the number of included words");
    if(mincard <= includedWordsNumber)    throw std::invalid_argument("maximum cardinality must be greater than the number of included words");

    //Computes the effective cardinalities
    effectiveMinCardinality = mincard - includedWordsNumber;
    effectiveMaxCardinality = maxcard - includedWordsNumber;
}

void ArmaMagna::setThreadsNumber()
{
    //Finds out the number of cpu cores
    supportedConcurrency = std::thread::hardware_concurrency();
    if(supportedConcurrency == 0) supportedConcurrency = 1;
}

void ArmaMagna::anagram()
{
    //Output settings
    this->print();

    //Reads the dictionary
    std::print("Reading dictionary...");
    try {dictionaryPtr = std::make_unique<Dictionarium>(dictionaryName, sourceText);}
    catch (std::invalid_argument &e) {throw std::invalid_argument(e.what());}
    std::print(" completed\n");
    std::print("Read {} words", dictionaryPtr->getWordsNumber());
    std::print(", filtered {}\n\n", dictionaryPtr->getWordsNumber() - dictionaryPtr->getEffectiveWordsNumber());

    {   //I/O thread RAII scope
        ioThread = std::jthread(&ArmaMagna::ioLoop, this); //I/O thread is launched

        //Computes the power set from the word lengths that are available in the dictionary after filtering
        std::vector<int> availableLengths = dictionaryPtr->getAvailableLengths();
        RepeatedCombinationsWithSum ps(targetSignature.getCharactersNumber(), effectiveMinCardinality, effectiveMaxCardinality, availableLengths);
        size_t powersetsNumber = ps.getSetsNumber();
        
        { 
            //int workersNumber = 1;
            int workersNumber = (supportedConcurrency > 2) ? supportedConcurrency - 2 : 1;  //2 threads reserved for main and I/O
            boost::asio::thread_pool pool (workersNumber);

            //std::println("[*] Starting {} threads", workersNumber);
            //std::println("[*] Covering {} powersets", powersetsNumber);

            //Search - Producer section
            for(size_t i=0; i<powersetsNumber; i++)
            {
                std::vector<int> set = ps.getSet(i);
                
                boost::asio::post(pool, [this, set]
                    {
                        SearchThread searchThread(*this, set);
                        searchThread();
                    }
                );
            }

            pool.join();
        }
    }   //I/O thread destroyed here

    //Signals the I/O thread that the search is complete
    searchIsComplete.store(true);
    anagramQueueCV.notify_one();
}

void ArmaMagna::ioLoop()
{
    std::string lastDisplayedAnagram = "";
    auto lastDisplayTime =  std::chrono::steady_clock::now();

    //Opens the output file
    outputFile.open("anagrams.txt", std::ios::out);
    if(!outputFile.is_open()) {std::print("[x] Can't open file"); exit(-1); /*TODO: expected value error handling*/}

    bool shouldTerminate = false;
    while(true)
    {
        std::string currentAnagram;

        /*******************CONSUMER CRITICAL SECTION*******************/
        {
            std::unique_lock<std::mutex> lock(anagramQueueMutex);
            
            //Wait for data to arrive or the termination signal
            auto exitFunction = [this] {return !anagramQueue.empty() || searchIsComplete.load();};
            anagramQueueCV.wait(lock, exitFunction);

            if(searchIsComplete.load() && anagramQueue.empty()) shouldTerminate = true; //Can't call "break" here because of lock RAII
            else if(!anagramQueue.empty())
            {
                anagramCount++;
                currentAnagram = anagramQueue.front();
                anagramQueue.pop();
            }
        } //Lock released

        if(shouldTerminate) break;

        /*******************I/O PROCESSING*******************/
        if(!currentAnagram.empty()) //If there's something, output to file
        {
            outputFile << currentAnagram << std::endl;
            outputFile.flush();
            lastDisplayedAnagram = currentAnagram;
        }

        //Update console every 1 second
        auto now = std::chrono::steady_clock::now();
        if(now - lastDisplayTime >= std::chrono::seconds(1))
        {
            std::print("\r[{}] {}{}", anagramCount, lastDisplayedAnagram, std::string(30, ' '));
            std::cout << std::flush;
            //std::cout << "\r" <<  << lastDisplayedAnagram << std::flush;
            lastDisplayTime = now;
        }
    }

    std::cout << "\n";
    outputFile.close();
}

void ArmaMagna::print()
{
    std::println("\nArmaMagna multi-threaded anagrammer engine\n");

    std::println("{:<25}{}", "[*] Source text:",               sourceText);
    std::println("{:<25}{}", "[*] Dictionary:",                dictionaryName);
    std::println("{:<25}{}", "[*] Included text:",             includedText.empty() ? "<void>" : includedText);
    std::println("{:<25}({},{})", "[*] Cardinality:",          minCardinality, maxCardinality);
    std::println("{:<25}{}", "[*] Estimated concurrency:",     supportedConcurrency);
    std::println("");

    std::println("{:<25}{}", "[*] Source text signature:",      sourceTextSignature.toString());
    std::println("{:<25}{}", "[*] Included words number:",      includedWordsNumber);
    std::println("{:<25}{}", "[*] Included text signature:",    includedText.empty() ? "<void>" : std::format("{}", includedTextSignature.toString()));
    std::println("{:<25}{}", "[*] Target signature:",           targetSignature.toString());
    std::println("{:<25}({},{})", "[*] Effective cardinality:", effectiveMinCardinality, effectiveMaxCardinality);
    std::println("");
}

//Counts words in a string
int countWords(const std::string &str)
{
    auto count = std::ranges::distance(
        str
        | std::views::split(' ')                                 
        | std::views::filter([](auto&& w){ return !w.empty(); }) //Remove empty splits
    );

    return static_cast<int>(count);
}