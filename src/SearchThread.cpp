#include <algorithm> //For std::find
#include <string>    //For std::string
#include <vector>    //For std::vector
#include <set>       //For std::set std::multiset

#include "SearchThread.h"

using namespace std;

SearchThread::SearchThread(ArmaMagna &armaMagna, const std::vector<int> &wordLengths)
    : armaMagna(armaMagna), wordLengths(wordLengths)
{
    //Allocates the 'solution' array, it will be filled with signatures that make a potential anagram
    solution = new const WordSignature*[armaMagna.targetSignature.getCharactersNumber()];

    wordsNumber = static_cast<int>(wordLengths.size());
}

SearchThread::~SearchThread()
{
    delete[] solution;
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
        auto it = armaMagna.anagramSet.find(orderedAnagram); //Looks for the anagram in the anagramSet
        if(it == armaMagna.anagramSet.end()) //If it's a new anagram
        {
            for(const string &word : unorderedAnagram) cout << word << " "; //Output
            cout << endl;
            armaMagna.anagramSet.emplace(orderedAnagram);
        }
        return;
    }

    //Recursive part
    const WordSignature &ws = *(solution[index]);
    const set<string> &words = armaMagna.dictionaryPtr->getWords(ws);
    for(const string &word : words)
    {
        orderedAnagram.emplace(word);     //Adds the word to the list
        unorderedAnagram.push_back(word);

        outputSolution(orderedAnagram, unorderedAnagram, index + 1); //Recursive call

        auto it = find(unorderedAnagram.begin(), unorderedAnagram.end(), word);
        unorderedAnagram.erase(it);
        orderedAnagram.erase(word); //Backtracking
    }
}
