#include <iostream>  //For I/O
#include <iomanip>   //For std::setw
#include <algorithm> //For std::find
#include <thread>    //For std::thread
#include <string>    //For std::string
#include <vector>    //For std::vector
#include <set>       //For std::set

#include "WordSignature.h"
#include "SmartDictionary.h"
#include "StringNormalizer.h"
#include "PowerSet.h"
#include "SearchThread.h"
#include "ArmaMagna.h"

using namespace std;

int countWords(const string &strString); //Counts words in a string

//Constructor
ArmaMagna::ArmaMagna(const string &sourceText, const string &dictionaryName, const string &includedText,
                     int minCardinality, int maxCardinality, int minWordLength, int maxWordLength)
{
    try
    {
        setSourceText(sourceText);
        setDictionaryName(dictionaryName);
        setIncludedText(includedText);
        setRestrictions(minCardinality, maxCardinality, minWordLength, maxWordLength);
    }
    catch(invalid_argument &e) {throw invalid_argument(e.what());}

    //Finds out the number of cpu cores
    concurrentThreadsSupported = thread::hardware_concurrency();
    if(concurrentThreadsSupported == 0) concurrentThreadsSupported = 1;
}

//Getters
const std::string &ArmaMagna::getSourceText()     const {return sourceText;}
const std::string &ArmaMagna::getDictionaryName() const {return dictionaryName;}
const std::string &ArmaMagna::getIncludedText()   const {return includedText;}
int ArmaMagna::getMinCardinality()                const {return minCardinality;}
int ArmaMagna::getMaxCardinality()                const {return maxCardinality;}
int ArmaMagna::getMinWordLength()                 const {return minWordLength;}
int ArmaMagna::getMaxWordLength()                 const {return maxWordLength;}

//Setters
void ArmaMagna::setSourceText(const string &sourceText)
{
    this->sourceText = sourceText;

    //Processes the source text and computes its signature
    string processedSourceText = StringNormalizer::normalize(sourceText);
    sourceTextSignature = WordSignature(processedSourceText);
}

void ArmaMagna::setDictionaryName(const string &dictionaryName)
{
    this->dictionaryName = dictionaryName;
}

void ArmaMagna::setIncludedText(const string &includedText)
{
    this->includedText = includedText;

    //Processes the included text
    string processedIncludedText = StringNormalizer::normalize(includedText);
    includedTextSignature = WordSignature(processedIncludedText);

    //Computes the numebr of included words
    if(includedText == "") includedWordsNumber = 0;
    else                   includedWordsNumber = countWords(includedText);

    //Invalid argument checking
    if(!includedTextSignature.isSubsetOf(sourceTextSignature)) throw invalid_argument("the included text must be a subset of the source text");
    if(sourceTextSignature == includedTextSignature)           throw invalid_argument("the included text is already an anagram of the source text");

    //target = source - included
    targetSignature = sourceTextSignature;
    targetSignature -= includedTextSignature;

    //Computes the effective cardinalities
    effectiveMinCardinality = minCardinality - includedWordsNumber;
    effectiveMaxCardinality = maxCardinality - includedWordsNumber;
}

void ArmaMagna::setRestrictions(int minCardinality, int maxCardinality, int minWordLength, int maxWordLength)
{
    this->minCardinality = minCardinality;
    this->maxCardinality = maxCardinality;
    this->minWordLength = minWordLength;
    this->maxWordLength = maxWordLength;

    //Arguments validity checking
    if(minCardinality <= 0 || maxCardinality <= 0) throw invalid_argument("cardinalities must be positive");
    if(minCardinality > maxCardinality)            throw invalid_argument("maximum cardinality must be greater or equal to minimum cardinality");
    if(minWordLength <=0 || maxWordLength <=0)     throw invalid_argument("word lengths must be positive");
    if(minWordLength > maxWordLength)              throw invalid_argument("maximum word length must be greater or equal to minimum word length");
    if(minCardinality <= includedWordsNumber)      throw invalid_argument("minimum cardinality must be greater than the number of included words");
    if(maxCardinality <= includedWordsNumber)      throw invalid_argument("maximum cardinality must be greater than the number of included words");

    //Computes the effective cardinalities
    effectiveMinCardinality = minCardinality - includedWordsNumber;
    effectiveMaxCardinality = maxCardinality - includedWordsNumber;
}

void ArmaMagna::anagram()
{
    //Output settings
    print(cout);

    //Reads the dictionary
    cout << "Reading dictionary...";
    try {dictionaryPtr = new SmartDictionary(dictionaryName, sourceText);}
    catch (invalid_argument &e) {throw invalid_argument(e.what());}
    cout << " completed\n";
    cout << "Read " << dictionaryPtr->getWordsNumber() << " words";
    cout << ", filtered " << dictionaryPtr->getWordsNumber() - dictionaryPtr->getEffectiveWordsNumber() << "\n" << endl;

    //Computes the power set
    int dictionaryLongestWordLength = dictionaryPtr->getLongestWordLength();
    if(maxWordLength > dictionaryLongestWordLength) maxWordLength = dictionaryLongestWordLength;
    PowerSet ps(targetSignature.getCharactersNumber(), effectiveMinCardinality, effectiveMaxCardinality, minWordLength, maxWordLength);

    //Search part
    for(int i=0; i<ps.getSetsNumber(); i++)
    {
        SearchThread searchThread(*this, ps.getSet(i));
        searchThread();
    }

    //Releases memory
    delete dictionaryPtr;
}

void ArmaMagna::print(ostream &os)
{
    cout << "\nArmaMagna multi-threaded anagrammer engine\n\n" << left;
    cout << setw(25) << "Source text: "             << sourceText                     << "\n";
    cout << setw(25) << "Dictionary:"               << dictionaryName                 << "\n";
    if(includedText == "") cout << setw(25) << "Included text: " << "<void>\n";
    else                   cout << setw(25) << "Included text: " << includedText      << "\n";
    cout << setw(25) << "Cardinality:"              << "(" << minCardinality          << ", " << maxCardinality << ")\n";
    cout << setw(25) << "Anagram words length:"     << "(" << minWordLength           << ", " << maxWordLength  << ")\n";
    cout << setw(25) << "Number of cores:"          << concurrentThreadsSupported     << "\n" << endl;

    cout << setw(25) << "Source text signature:"    << sourceTextSignature            << "\n";
    cout << setw(25) << "Included words number:"    << includedWordsNumber            << "\n";
    if(includedText == "") cout << setw(25) << "Included text signature: " << "<void>\n";
    else                   cout << setw(25) << "Included text signature: " << includedTextSignature << "\n";
    cout << setw(25) << "Target signature: "        << targetSignature                << "\n";
    cout << setw(25) << "Effective cardinality:"    << "(" << effectiveMinCardinality << ", " << effectiveMaxCardinality << ")\n" << endl;
}

//Counts words in a string
int countWords(const string &str)
{
    size_t i = 0;
    int result = 0;

    while(i < str.length() && str.at(i) == ' ') i++; //Skips initial spaces

    while(i != str.length())
    {
        while(i < str.length() && isalpha(str.at(i))) i++; //Skips a word
        result++;

        while(i < str.length() && str.at(i) == ' ') i++; //Skips spaces
    }

    return result;
}
