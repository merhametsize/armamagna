#include <algorithm> //For std::find
#include <cassert>   //For assert
#include <string>    //For std::string
#include <vector>    //For std::vector
#include <mutex>     //For std::mutex, std::lock_guard
#include <set>       //For std::set std::multiset

#include "SearchThread.h"

using namespace std;

SearchThread::SearchThread(ArmaMagna &armaMagna, const std::vector<int> wordLengths)
    : armaMagna(armaMagna), wordLengths(wordLengths)
{
    //Modifies the size of the 'solution' vector, it will be filled with signatures that make a potential anagram
    wordsNumber = static_cast<int>(wordLengths.size());
    solution.resize(wordsNumber);

    assert(wordsNumber > 0);
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

        solution[wordIndex] = &currentSignature; //Saves a pointer to the current signature in the 'solution' array
        assert(wordIndex >= 0 && wordIndex < wordsNumber);

        search(wordIndex + 1); //Recursive call

        ws -= currentSignature; //Backtracking
    }
}

void SearchThread::computeSolution()
{
    //Creates and ordered anagram and unordered one
    multiset<string> orderedAnagram; //This one will be put in the anagramSet set, in order to avoid duplicates
    vector<string> unorderedAnagram; //This one will be output

    //Adds the included words to the vector, if there's any
    if(armaMagna.includedText != "")
    {
        orderedAnagram.emplace(armaMagna.includedText);
        unorderedAnagram.push_back(armaMagna.includedText);
    }

    //Adds the found words to the set
    outputSolution(orderedAnagram, unorderedAnagram, 0); //Recursive function
}

void SearchThread::outputSolution(multiset<string> &orderedAnagram, vector<string> &unorderedAnagram, int index)
{
    //Base case
    if(index == wordsNumber)
    {
        std::string outputString;
        bool shouldPush = false;

        //Formats the output string
        for(const std::string &word : unorderedAnagram) {outputString += word; outputString += " ";}
        assert(!outputString.empty());
        outputString.pop_back(); //Trailing space is removed

        //PRODUCER CRITICAL SECTION
        {
            std::scoped_lock lock(armaMagna.anagramSetMutex, armaMagna.anagramQueueMutex);

            auto it = armaMagna.anagramSet.find(orderedAnagram); //Looks for the anagram in the anagramSet
            if(it == armaMagna.anagramSet.end()) //If it's a new anagram
            {
                armaMagna.anagramSet.emplace(orderedAnagram);
                armaMagna.anagramQueue.push(std::move(outputString)); //Result is pushed to the I/O queue
                shouldPush = true;
            }
        } //Critical section ends

        if(shouldPush) armaMagna.anagramQueueCV.notify_one(); //If the anagram is not new, don't notify
        return;
    }

    //Recursive part
    assert(index >= 0 && index < solution.size());
    const WordSignature &ws = *(solution[index]);
    const set<string> &words = armaMagna.dictionaryPtr->getWords(ws);
    for(const string &word : words)
    {
        orderedAnagram.emplace(word);     //Adds the word to the list
        unorderedAnagram.push_back(word);

        outputSolution(orderedAnagram, unorderedAnagram, index + 1); //Recursive call

        unorderedAnagram.pop_back();                        //O(1) vector backtracking
        orderedAnagram.erase(orderedAnagram.find(word));   //O(log N) multiset backtracking
    }
}
