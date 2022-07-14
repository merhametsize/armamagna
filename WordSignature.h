#ifndef FREQUENCY_TABLE_H
#define FREQUENCY_TABLE_H

#include <string>  //For std::string
#include <map>     //For std::map

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
    bool operator< (const WordSignature &ws) const;
    bool operator> (const WordSignature &ws) const;
    bool operator==(const WordSignature &ws) const;
    bool isSubsetOf(const WordSignature &ws) const;

    //Getters
    int getCharactersNumber() const;

private:
    std::map<char, int> table;
    std::string signature;
};

#endif
