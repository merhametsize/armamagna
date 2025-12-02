#ifndef WORD_SIGNATURE_H
#define WORD_SIGNATURE_H

#include <string>  //For std::string
#include <ranges>  //For std::views
#include <array>   //For std::array

class WordSignatureHash;

class WordSignature
{
    friend std::ostream &operator<<(std::ostream &ostream, const WordSignature &ws);
    friend struct std::hash<WordSignature>; 

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

template <>
struct std::hash<WordSignature>
{
    ~hash() = default; 
    hash() = default;

    size_t operator()(const WordSignature& ws) const noexcept
    {
        size_t seed = 0;
        std::hash<int> hasher;
        
        //This constant is a magic number used in hash combining
        const size_t HASH_MAGIC = 0x9e3779b9; 

        //Combine the current seed with the hash of the current count
        for (int count : ws.table) {seed ^= hasher(count) + HASH_MAGIC + (seed << 6) + (seed >> 2);}
        
        return seed;
    }
};

#endif
