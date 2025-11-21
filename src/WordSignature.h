#ifndef FREQUENCY_TABLE_H
#define FREQUENCY_TABLE_H

#include <string>  //For std::string
#include <ranges>  //For std::views
#include <array>   //For std::array

class WordSignature
{
    //Output debug function
    friend std::ostream &operator<<(std::ostream &ostream, const WordSignature &ws);

public:
    explicit WordSignature();
    explicit WordSignature(const std::string &string);

    //Operators functions
    void operator+=(const WordSignature &ws);
    void operator-=(const WordSignature &ws);
    auto operator<=>(const WordSignature &ws) const = default;
    bool isSubsetOf(const WordSignature &ws) const;

    //Getters
    int getCharactersNumber() const;

    //Transformers
    std::string toString() const;

private:
    std::array<int, 26> table = {}; //Initialized to 0
};


#endif
