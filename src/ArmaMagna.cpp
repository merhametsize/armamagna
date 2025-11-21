#include <iostream>  //For I/O
#include <algorithm> //For std::find
#include <memory>    //For std::make_unique, std::unique_ptr
#include <string>    //For std::string
#include <vector>    //For std::vector
#include <chrono>    //For std::chrono
#include <cctype>    //For std::isspace
#include <ranges>    //For std::views, std::ranges::distance
#include <format>    //For std::format
#include <print>     //For std::print
#include <set>       //For std::set

#include "WordSignature.h"
#include "Dictionarium.h"
#include "StringNormalizer.h"
#include "PowerSet.h"
#include "SearchThread.h"
#include "ArmaMagna.h"
#include "ThreadPool.h"

int countWords(const std::string &str); //Counts words in a string

//Constructor
ArmaMagna::ArmaMagna(const std::string &sourceText, const std::string &dictionaryName, const std::string &includedText,
                     int minCardinality, int maxCardinality)
{
    try
    {
        setSourceText(sourceText);
        setDictionaryName(dictionaryName);
        setIncludedText(includedText);
        setRestrictions(minCardinality, maxCardinality);
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
void ArmaMagna::setSourceText(const std::string &sourceText)
{
    this->sourceText = sourceText;

    //Processes the source text and computes its signature
    std::string processedSourceText = StringNormalizer::normalize(sourceText);
    sourceTextSignature = WordSignature(processedSourceText);
}

void ArmaMagna::setDictionaryName(const std::string &dictionaryName)
{
    this->dictionaryName = dictionaryName;
}

void ArmaMagna::setIncludedText(const std::string &includedText)
{
    this->includedText = includedText;

    //Processes the included text
    std::string processedIncludedText = StringNormalizer::normalize(includedText);
    includedTextSignature = WordSignature(processedIncludedText);

    //Computes the number of included words
    if(includedText == "") includedWordsNumber = 0;
    else                   includedWordsNumber = countWords(includedText);

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

void ArmaMagna::setRestrictions(int minCardinality, int maxCardinality)
{
    this->minCardinality = minCardinality;
    this->maxCardinality = maxCardinality;

    //Arguments validity checking
    if(minCardinality <= 0 || maxCardinality <= 0) throw std::invalid_argument("cardinalities must be positive");
    if(minCardinality > maxCardinality)            throw std::invalid_argument("maximum cardinality must be greater or equal to minimum cardinality");
    if(minCardinality <= includedWordsNumber)      throw std::invalid_argument("minimum cardinality must be greater than the number of included words");
    if(maxCardinality <= includedWordsNumber)      throw std::invalid_argument("maximum cardinality must be greater than the number of included words");

    //Computes the effective cardinalities
    effectiveMinCardinality = minCardinality - includedWordsNumber;
    effectiveMaxCardinality = maxCardinality - includedWordsNumber;
}

void ArmaMagna::setThreadsNumber()
{
    //Finds out the number of cpu cores
    concurrentThreadsSupported = std::thread::hardware_concurrency();
    if(concurrentThreadsSupported == 0) concurrentThreadsSupported = 1;
}

void ArmaMagna::anagram()
{
    //Output settings
    print(std::cout);

    //Reads the dictionary
    std::print("Reading dictionary...");
    try {dictionaryPtr = std::make_unique<Dictionarium>(dictionaryName, sourceText);}
    catch (std::invalid_argument &e) {throw std::invalid_argument(e.what());}
    std::print(" completed\n");
    std::print("Read {} words", dictionaryPtr->getWordsNumber());
    std::print(", filtered {}\n", dictionaryPtr->getWordsNumber() - dictionaryPtr->getEffectiveWordsNumber());

    {
        //Launches the I/O thread
        ioThread = std::jthread(&ArmaMagna::ioLoop, this);

        //Computes the power set
        int dictionaryLongestWordLength = dictionaryPtr->getLongestWordLength();
        PowerSet ps(targetSignature.getCharactersNumber(), effectiveMinCardinality, effectiveMaxCardinality, 1, dictionaryLongestWordLength);
        size_t powersetsNumber = ps.getSetsNumber();

        std::cout << ps << std::endl;
        
        {   //Scope for ThreadPool RAII
            int workersNumber = (concurrentThreadsSupported > 2) ? concurrentThreadsSupported - 2 : 1;  //-2 for main thread and I/O thread
            ThreadPool pool(workersNumber);

            std::println("[*] Starting {} threads", workersNumber);
            std::println("[*] Covering {} powersets", powersetsNumber);

            //Search - Producer section
            for(size_t i=0; i<powersetsNumber; i++)
            {
                std::vector<int> set = ps.getSet(i);
                pool.enqueue([this, set] 
                {
                    SearchThread searchThread(*this, set);
                    searchThread();
                    //std::cout << "I am the thread for set" << set.data() << std::endl;
                });
            }
        }   //Thread pool destroyed here - All threads are joined
    }

    //Signal the I/O thread that the search is complete
    searchIsComplete.store(true);
    anagramQueueCV.notify_one();
}

void ArmaMagna::ioLoop()
{
    std::print("Entered ioloop");

    std::string lastDisplayedAnagram = "";
    auto lastDisplayTime =  std::chrono::steady_clock::now();

    //Opens the output file
    outputFile.open("anagrams.txt", std::ios::out);
    if(!outputFile.is_open()) {std::print("[x] Can't open file"); exit(-1); /*TODO: expected error handling*/}

    while(true)
    {
        std::string currentAnagram;
        bool shouldTerminate = false;

        /*******************CONSUMER CRITICAL SECTION*******************/
        {
            std::unique_lock<std::mutex> lock(anagramQueueMutex);
            
            //Wait for data to arrive or the termination signal
            auto exitFunction = [this] {return !anagramQueue.empty() || searchIsComplete.load();};
            anagramQueueCV.wait_for(lock, std::chrono::milliseconds(100), exitFunction);

            if(searchIsComplete.load() && anagramQueue.empty()) shouldTerminate = true; //Can't call "break" here because of lock RAII
            else if (!anagramQueue.empty()) 
            {
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
            std::cout << "\r" << lastDisplayedAnagram << std::flush;
            lastDisplayTime = now;
        }
    }

    std::print("Exiting ioloop");

    outputFile.close();
}

void ArmaMagna::print(std::ostream &os)
{
    std::println("\nArmaMagna multi-threaded anagrammer engine\n");

    std::println("{:<25}{}", "[*] Source text:",               sourceText);
    std::println("{:<25}{}", "[*] Dictionary:",                dictionaryName);
    std::println("{:<25}{}", "[*] Included text:",             includedText.empty() ? "<void>" : includedText);
    std::println("{:<25}({},{})", "[*] Cardinality:",          minCardinality, maxCardinality);
    std::println("{:<25}{}", "[*] Estimated concurrency:",     concurrentThreadsSupported);
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
    return std::ranges::distance(
          str
        | std::views::split(' ')                                 
        | std::views::filter([](auto&& w){ return !w.empty(); }) //Remove empty splits
    );
}