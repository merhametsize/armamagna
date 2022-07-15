#ifndef STRING_NORMALIZER_H
#define STRING_NORMALIZER_H

#include <string> //For std::string

class StringNormalizer
{
public:
    //Normalizes a string to ASCII alphabetic sequence
    //Example: per-ché -----> perche
    static std::string normalize(const std::string &s);
};

#endif
