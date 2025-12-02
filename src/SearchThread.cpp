#include <algorithm> //For std::sort
#include <cassert>   //For assert
#include <string>    //For std::string
#include <vector>    //For std::vector
#include <mutex>     //For std::mutex, std::lock_guard
#include <print>     //For std::println
#include <set>       //For std::set std::multiset

#include "SearchThread.h"

SearchThread::SearchThread(ArmaMagna &am, const std::vector<int>& wl)
    : armaMagna(am), wordLengths(wl)
{
    //Modifies the size of the 'solution' vector, it will be filled with signatures that make a potential anagram
    wordsNumber = static_cast<int>(wordLengths.size());
    assert(wordsNumber > 0);
    solution.resize(wordsNumber);
}   

void SearchThread::operator()()
{
    //Calls the searching algorithm with initial wordIndex 0
    //wordIndex tells us which integer of 'wordLenghts' shall be used
    search(0);
}

void SearchThread::search(int wordIndex)
{
    //Base case, possible solution found
    if(wordIndex >= wordsNumber)
    {
        if(ws == armaMagna.targetSignature) computeSolution();
        return;
    }

    //Iterates through every entry of the dictionary section contained in wordLengths[wordsIndex]
    for(const auto &entry : armaMagna.dictionaryPtr->getSection(wordLengths[wordIndex]))
    {
        const WordSignature &currentSignature = entry.first; //Gets the current signature

        ws += currentSignature;   //Adds the current entry's signature to ws
        if(wordIndex >= 1)       //Pruning block
        {
            if(!ws.isSubsetOf(armaMagna.targetSignature)) {ws -= currentSignature; continue;}
        }

        solution[wordIndex] = currentSignature; //Saves a pointer to the current signature in the 'solution' array
        assert(wordIndex >= 0 && wordIndex < wordsNumber);

        search(wordIndex + 1); //Recursive call

        ws -= currentSignature; //Backtracking
    }
}

void SearchThread::computeSolution()
{
    //Creates and ordered anagram and unordered one
    std::multiset<std::string> orderedAnagram; //This one will be put in the anagramSet set, in order to avoid duplicates
    std::vector<std::string> unorderedAnagram; //This one will be output

    //Adds the included words to the vector, if there's any
    if(armaMagna.includedText != "")
    {
        orderedAnagram.emplace(armaMagna.includedText);
        unorderedAnagram.push_back(armaMagna.includedText);
    }

    //Adds the found words to the set
    outputSolution(orderedAnagram, unorderedAnagram, 0); //Recursive function
}

void SearchThread::outputSolution(std::multiset<std::string> &orderedAnagram, std::vector<std::string> &unorderedAnagram, int index)
{
    //Base case
    if(index == wordsNumber)
    {
        std::string canonicalString;
        bool shouldPush = false;

        //Formats the output string
        std::sort(unorderedAnagram.begin(), unorderedAnagram.end());
        for(const std::string &word : unorderedAnagram) {canonicalString += word; canonicalString += " ";}
        assert(!canonicalString.empty());  
        canonicalString.pop_back(); //Trailing space is removed

        //PRODUCER CRITICAL SECTION
        {
            std::lock_guard lock(armaMagna.anagramQueueMutex);

            if(anagramSet.insert(canonicalString).second) //If the insertion took place, it's a new anagram
            {
                armaMagna.anagramQueue.push(std::move(canonicalString)); //Result is pushed to the I/O queue
                shouldPush = true;
            }
        } //Critical section ends

        if(shouldPush) armaMagna.anagramQueueCV.notify_one(); //If the anagram is not new, don't notify
        return;
    }

    //Recursive part
    assert(index >= 0 && index < static_cast<int>(solution.size()));
    const WordSignature wordSignature = solution[index];
    const std::vector<std::string> &words = armaMagna.dictionaryPtr->getWords(wordSignature);
    for(const std::string &word : words)
    {
        orderedAnagram.emplace(word);     //Adds the word to the list
        unorderedAnagram.push_back(word);

        outputSolution(orderedAnagram, unorderedAnagram, index + 1); //Recursive call

        unorderedAnagram.pop_back();                        //O(1) vector backtracking
        orderedAnagram.erase(orderedAnagram.find(word));   //O(log N) multiset backtracking
    }
}
