#include <iostream>        //For I/O functions
#include <algorithm>       //For std::sort
#include <cassert>         //For assert
#include <numeric>         //For std::accumulate
#include <ranges>          //For c++23 std::views::enumerate
#include <string>          //For std::string
#include <array>           //For std::array

#include "WordSignature.h"

//Default constructor
WordSignature::WordSignature() : WordSignature("")
{
    //emtpy
    //Initialization is delegated to WordSignature(const std::string &)
}

//Constructor
WordSignature::WordSignature(const std::string &word)
{
    //Creates the table
    for(const char c : word)  //For each character in the string
    {
        //O(1) table access
        assert(c>='a' && c<='z'); //Ensures the character is alphabetical
        table[c - 'a']++; //a-a=0, c-a=2 etc, assuming 'c' is normalized
    }
}

//Add function
void WordSignature::operator+=(const WordSignature &ws)
{
    for(const auto [i, count] : ws.table | std::views::enumerate) //Pipes the container through the enumerate adaptor
    {
        table[i] += count;
    }
}

//Sub function
void WordSignature::operator-=(const WordSignature &ws)
{
    for(const auto [i, count] : ws.table | std::views::enumerate) //Pipes the container through the enumerate adaptor
    {
        table[i] -= count;
        assert(table[i] >= 0);
    }
}

//Determines whether this object's signature is a subset of another
bool WordSignature::isSubsetOf(const WordSignature &ws) const
{
    for (int i = 0; i < 26; ++i)
    {
        if (this->table[i] > ws.table[i]) return false;
    }
    return true;
}

//Getters
int WordSignature::getCharactersNumber() const
{
    return std::accumulate(table.begin(), table.end(), 0);
}

std::string WordSignature::toString() const
{
    std::string str;
    for (const auto &[i, count] : this->table | std::views::enumerate) // C++23 range-for loop with enumerate
    {
        if (count > 0)
        {
            char c = static_cast<char>('a' + i);
            assert(count <= 40);
            str.append(count, c);
        }
    }
    return str;
}

//Output debug function
std::ostream &operator<<(std::ostream &ostream, const WordSignature &ws)
{
    return ostream << ws.toString();
}
