#include <iostream>        //For I/O functions
#include <string>          //For std::string
#include <algorithm>       //For std::sort
#include <map>             //For std::map
#include "WordSignature.h"

using namespace std;

//Default constructor
WordSignature::WordSignature() : WordSignature("")
{
    //emtpy
    //Initialization is delegated to WordSignature(const std::string &)
}

//Constructor
WordSignature::WordSignature(const string &word)
{
    //Creates the table
    for(const char &c : word)  //For each character in the string
    {
        auto it = table.find(c);                              //Looks for c in the map
        if(it != table.end()) it->second = it->second + 1;   //If the character is already in the map, add 1 to its frequency
        else                  table.emplace(c, 1);          //Otherwise add the character with frequency 1

        signature += c;
    }
    sort(signature.begin(), signature.end());
}

//Add function
void WordSignature::operator+=(const WordSignature &ws)
{
    for(const auto &entry : ws.table) //For each entry in the other ws
    {
        char c = entry.first;
        int f = entry.second;

        auto it = table.find(c);         //Looks for c in the map
        if(it != table.end())           //If it's present
        {
            int oldF = (*it).second;  //Gets the old frequency
            (*it).second = oldF + f; //Adds the old frequency with the new one
        }
        else table.emplace(c, f);  //Otherwise insert a new (c, f) entry

        /****************************************************************
        *                          NOT NEEDED                           *
        * These lines will refresh the signature, thus keeping coherent *
        * the table and the signature, but it's actually not needed for *
        * this program's purposes, and it would significantly slow down *
        * the anagram search.                                           *
        *****************************************************************/
        //signature += c;                        //Adds the character to the signature
    }
    //sort(signature.begin(), signature.end()); //Keeps sorted the signature's characters
}

//Sub function
void WordSignature::operator-=(const WordSignature &ws)
{
    for(const auto &entry : ws.table) //For each entry in the other ws
    {
        char c = entry.first;        //Gets the character
        int f = entry.second;       //Gets its frequence

        auto it = table.find(c);  //Looks for c in the map
        if(it != table.end())    //If it's present
        {
            int oldF = (*it).second;                    //Gets the old frequency
            if(oldF - f == 0) table.erase(c);          //If the frequencies have null sum, erases the entry
            else              it->second = oldF - f;  //Otherwise subtract the old frequency with the new one
        }
        else table.emplace(c, (-1) * f);   //Otherwise insert a new (c, -f) entry

        /****************************************************************
        *                          NOT NEEDED                           *
        * These lines will refresh the signature, thus keeping coherent *
        * the table and the signature, but it's actually not needed for *
        * this program's purposes, and it would significantly slow down *
        * the anagram search.                                           *
        *****************************************************************/
        //auto it = find(signature.begin(), signature.end(), c); //Looks for the character in the signature
        //signature.erase(it);                                   //Erases it
    }
}

//'Lesser than' operator function
bool WordSignature::operator<(const WordSignature &ws) const
{
    return this->signature < ws.signature;
}

//'Greater than' operator function
bool WordSignature::operator>(const WordSignature &ws) const
{
    return !(*this < ws); //Delegates the job to operator<
}

//'Greater than' operator function
bool WordSignature::operator==(const WordSignature &ws) const
{
    return (this->table == ws.table);
}

//Determines whether a signature is a subset of another
bool WordSignature::isSubsetOf(const WordSignature &ws) const
{
    for(const auto &entry : this->table)
    {
        auto it = ws.table.find(entry.first);                                   //Looks for the current key in the second signature
        if(it != ws.table.end()) {if(entry.second > it->second) return false;} //If it's present, checks if freq1 < freq2
        else return false;                                                    //If it's not present, it's not a subset
    }
    return true;
}

//Getters
int WordSignature::getCharactersNumber() const
{
    int n = 0;

    for(const auto &entry : table) n += entry.second;
    return n;
}

//Output debug function
ostream &operator<<(ostream &ostream, const WordSignature &ws)
{
    string signature;

    for(const auto &entry : ws.table) for(int i=0; i<entry.second; i++) signature += entry.first; //Creates the signature
    sort(signature.begin(), signature.end()); //Sorts the characters in the signature. This is needed in case the map is a hash table
    cout << signature;

    return ostream;
}
