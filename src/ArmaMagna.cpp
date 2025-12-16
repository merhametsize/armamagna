#include <iostream>  //For I/O
#include <expected>  //For std::expected, std::unexpected
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
ArmaMagna::ArmaMagna()
{
    //Empty
}

auto ArmaMagna::setOptions(const std::string &text, const std::string &dictionary, const std::string& outputFileName,
    const std::string &included, int mincard, int maxcard, int numThreads)
    -> std::expected<void, std::string>
{
    auto ret = setTargetText(text);
    if(!ret) {return std::unexpected(ret.error());}

    ret = setIncludedText(included);
    if(!ret) {return std::unexpected(ret.error());}

    ret = setRestrictions(mincard, maxcard);
    if(!ret) {return std::unexpected(ret.error());}

    setDictionaryName(dictionary);
    setThreadsNumber(numThreads);

    this->outputFileName = outputFileName;

    return {};
}

//Setters
auto ArmaMagna::setTargetText(const std::string text) -> std::expected<void, std::string>
{
    this->targetText = text;

    //Processes the target text and computes its signature
    auto processedTargetText = StringNormalizer::normalize(text);
    if(!processedTargetText) {return std::unexpected(processedTargetText.error());}
    this->targetSignature = WordSignature(processedTargetText.value());

    return {};
}

void ArmaMagna::setDictionaryName(const std::string dictionary)
{
    this->dictionaryName = dictionary;
}

auto ArmaMagna::setIncludedText(const std::string included) -> std::expected<void, std::string>
{
    this->includedText = included;

    //Processes the included text
    auto processedIncludedText = StringNormalizer::normalize(included);
    if(!processedIncludedText) {return std::unexpected(processedIncludedText.error());}
    includedTextSignature = WordSignature(processedIncludedText.value());

    //Computes the number of included words
    if(included == "") includedWordsNumber = 0;
    else               includedWordsNumber = countWords(included);

    //Invalid argument checking
    if(!includedTextSignature.isSubsetOf(targetSignature)) {return std::unexpected("The included text must be a subset of the target text");}
    if(targetSignature == includedTextSignature)           {return std::unexpected("The included is an anagram of the target text");}

    //actual = target - included
    actualTargetSignature = targetSignature; 
    actualTargetSignature -= includedTextSignature;

    //Computes the effective cardinalities
    actualMinCardinality = minCardinality - includedWordsNumber;
    actualMaxCardinality = maxCardinality - includedWordsNumber;

    return {};
}

auto ArmaMagna::setRestrictions(int mincard, int maxcard) -> std::expected<void, std::string>
{
    //Arguments validity checking
    if(mincard <= 0 || maxcard <= 0)      {return std::unexpected("Cardinalities must be positive");}
    if(mincard > maxcard)                 {return std::unexpected("Maximum cardinality must be greater or equal than minimum cardinality");}
    if(mincard <= includedWordsNumber)    {return std::unexpected("Minimum cardinality must be >= than the number of included words");}
    if(maxcard <= includedWordsNumber)    {return std::unexpected("Maximum cardinality must be >= than the number of included words");}

    this->minCardinality = mincard;
    this->maxCardinality = maxcard;

    //Computes the actual cardinalities
    actualMinCardinality = mincard - includedWordsNumber;
    actualMaxCardinality = maxcard - includedWordsNumber;

    return {};
}

void ArmaMagna::setThreadsNumber(unsigned int n)
{
    if(n > std::thread::hardware_concurrency()) this->numThreads = std::thread::hardware_concurrency();
    else if(n <= 0) this->numThreads = 1;
    this->numThreads = n;
}

auto ArmaMagna::anagram() -> std::expected<unsigned long long, std::string>
{
    //Output settings
    this->print();

    //Reads the dictionary
    auto wordsRead = dictionary.readWordList(dictionaryName, targetText);
    if(!wordsRead) {return std::unexpected(wordsRead.error());}
    std::print("[*] Read {} words from dictionary, ", wordsRead.value());
    std::print(", after filter {}\n\n", dictionary.getActualWordsNumber());

    //Opens the output file
    this->ofstream.open(this->outputFileName, std::ios::out);
    if(!this->ofstream.is_open()) {return std::unexpected("Cannot open output file");}

    auto startTime = std::chrono::steady_clock::now();
    {   //I/O thread RAII scope
        ioThread = std::jthread(&ArmaMagna::ioLoop, this);

        //Computes the power set from the word lengths that are available in the dictionary after filtering
        std::vector<int> availableLengths = dictionary.getAvailableLengths();
        RepeatedCombinationsWithSum rcs(actualTargetSignature.getCharactersNumber(), actualMinCardinality, actualMaxCardinality, availableLengths);
        this->setsNumber = rcs.getSetsNumber();
        
        int workersNumber = (numThreads > 2) ? numThreads - 2 : 1;  //2 threads reserved for main and I/O
        boost::asio::thread_pool pool(workersNumber);

        std::println("[*] Starting {} search threads", workersNumber);
        std::println("[*] Covering {} length combinations\n", this->setsNumber);

        //Search - Producer section
        for(size_t i=0; i<this->setsNumber; i++)
        {
            std::vector<int> set = rcs.getSet(i);
            
            boost::asio::post(pool, [this, set]
                {
                    SearchThread searchThread(*this, set);
                    searchThread();
                    this->exploredSetsNumber++; //☢️
                }
            );
        }

        pool.join();
    }   //I/O thread destroyed here

    //Signals the I/O thread that the search is complete
    searchIsComplete.store(true);
    anagramQueueCV.notify_one();

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
    std::println("[*] Search time: {:.2f} s", elapsed.count()/1000);

    this->ofstream.close();
    return this->anagramCount;
}

void ArmaMagna::ioLoop()
{
    auto lastDisplayTime =  std::chrono::steady_clock::now();

    bool shouldTerminate = false;
    std::string currentAnagram;
    while(true)
    {
        /*******************CONSUMER CRITICAL SECTION*******************/
        {
            std::unique_lock<std::mutex> lock(anagramQueueMutex); //unique_lock is needed because of CV.wait()
            
            //Wait for data to arrive or the termination signal
            auto exitFunction = [this] {return !anagramQueue.empty() || searchIsComplete.load();};
            anagramQueueCV.wait_for(lock, std::chrono::milliseconds(1000), exitFunction);

            if(searchIsComplete.load() && anagramQueue.empty()) shouldTerminate = true; //Can't call "break" here because of lock RAII
            else if(!anagramQueue.empty())
            {
                this->anagramCount++;
                currentAnagram = anagramQueue.front();
                anagramQueue.pop();
            }
        } //Lock released

        if(shouldTerminate) break;

        /*******************I/O PROCESSING*******************/
        if(!currentAnagram.empty()) //If there's something, output to file
        {
            this->ofstream << currentAnagram << std::endl;
            this->ofstream.flush();
        }

        //Update console every 1 second
        auto now = std::chrono::steady_clock::now();
        if(now - lastDisplayTime >= std::chrono::milliseconds(1000))
        {
            std::print("\r[{}/{} sets] {}: {}{}", this->exploredSetsNumber.load(), this->setsNumber, this->anagramCount, currentAnagram, std::string(30, ' '));
            std::cout << std::flush;
            lastDisplayTime = now;
        }
    }

    std::print("\r[{}/{} sets] {}: {}{}", this->exploredSetsNumber.load(), this->setsNumber, this->anagramCount, currentAnagram, std::string(30, ' '));
    std::println("\n\n[*] Found {} anagrams, output in {}", this->anagramCount, this->outputFileName);
    std::cout << std::flush;
    return;
}

void ArmaMagna::print()
{
    std::println("\nArmaMagna multi-threaded anagrammer engine\n");

    std::println("{:<40}{}", "[*] Target text:",               targetText);
    std::println("{:<40}{}", "[*] Dictionary:",                dictionaryName);
    std::println("{:<40}{}", "[*] Included text:",             includedText.empty() ? "<void>" : includedText);
    std::println("{:<40}({},{})", "[*] Cardinality:",          minCardinality, maxCardinality);
    std::println("{:<40}{}", "[*] Estimated concurrency:",     std::thread::hardware_concurrency());
    std::println("{:<40}{}", "[*] Threads to launch:",         numThreads);
    std::println("");

    std::println("{:<40}{}", "[*] Target signature:",           targetSignature.toString());
    std::println("{:<40}{}", "[*] Included words number:",      includedWordsNumber);
    std::println("{:<40}{}", "[*] Included text signature:",    includedText.empty() ? "<void>" : std::format("{}", includedTextSignature.toString()));
    std::println("{:<40}{}", "[*] Actual target signature:",    actualTargetSignature.toString());
    std::println("{:<40}({},{})", "[*] Actual cardinality:",    actualMinCardinality, actualMaxCardinality);
    std::println("");
}

//Counts words in a string
int countWords(const std::string& str)
{
    auto count = std::ranges::distance(
        str
        | std::views::split(' ')                                 
        | std::views::filter([](auto&& w){ return !w.empty(); }) //Remove empty splits
    );

    return static_cast<int>(count);
}