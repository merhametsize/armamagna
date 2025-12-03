#ifndef STRING_NORMALIZER_H
#define STRING_NORMALIZER_H

#include <string> //For std::string

class StringNormalizer
{
public:
    //Normalizes a string to ASCII alphabetic sequence
    //Example: per-ché -----> perche
    static auto normalize(const std::string &s) -> std::expected<std::string, std::string>;
};

#endif
