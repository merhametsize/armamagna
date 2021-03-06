#ifndef SEARCH_H
#define SEARCH_H

#include <string> //For std::string
#include <vector> //For std::vector
#include <set>    //For std::multiset

#include "WordSignature.h"
#include "ArmaMagna.h"

class SearchThread
{
public:
    SearchThread(ArmaMagna &armaMagna, const std::vector<int> &wordLengths);
    ~SearchThread();
    void operator()();

private:
    //Constructor arguments
    ArmaMagna &armaMagna;                 //Reference to the friend armaMagna object, needed to access its dictionary and the anagram list
    const std::vector<int> &wordLengths; //Vector of integers generated by PowerSet, every int represents a word length

    //Other variables
    int wordsNumber;                  //Number of words of which the anagram will be made of
    WordSignature ws;                //Dummy signature, used in the recursive search algorithm
    const WordSignature **solution; //Array of pointers to a signature, will contain the pointers to signatures that make a solution

    void search(int wordIndex);   //Main function, core of the program
    void computeSolution();
    void outputSolution(std::multiset<std::string> &orderedAnagram, std::vector<std::string> &unorderedAnagram, int index);
};

#endif
