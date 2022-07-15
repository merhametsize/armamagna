#include <iostream>
#include <stdexcept>

#include "StringNormalizer.h"

using namespace std;

//Normalizes a string to ASCII alphabetic sequence
//Example: per-ché -----> perche
string StringNormalizer::normalize(const string &s)
{
    string result;

    for(size_t i=0; i<s.length(); i++)
    {
        char c = s.at(i); //Gets the character
        if(c == '\xc3')  //If this byte is 0xc3 (uint), then it's a unicode sequence to normalize
        {
            char nc = s.at(i+1); //Gets the next byte
            if     (nc >= '\xa0' && nc <= '\xa5') result += 'a';
            else if(nc >= '\xa8' && nc <= '\xab') result += 'e';
            else if(nc == '\xa6')                 result += "ae";
            else if(nc >= '\xac' && nc <= '\xaf') result += 'i';
            else if(nc >= '\xb2' && nc <= '\xb6') result += 'o';
            else if(nc >= '\xb9' && nc <= '\xbc') result += 'u';
            else if(nc >= '\xbd' && nc <= '\xbf') result += 'y';
            else if(nc == '\xa7')                 result += 'c';
            else if(nc == '\xb1')                 result += 'n';
            else //If the unicode sequence is not among those above, throws an exception
            {
                string msg = "cannot normalize a character in string: \"";
                msg += s; msg += "\"";
                throw invalid_argument(msg);
            }

            i += 1;
        }
        else if(!isalpha(c)) {}          //Ignores non-alphabetic characters
        else result += std::tolower(c); //Lowers the alphabetic character
    }
    return result;
}
