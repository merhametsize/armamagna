#ifndef WORD_SIGNATURE_H
#define WORD_SIGNATURE_H

#include <cstdint>   //For uint8_t
#include <string>   //For std::string
#include <array>   //For std::array

class WordSignature
{
    friend std::ostream& operator<<(std::ostream& ostream, const WordSignature& ws);
    friend struct std::hash<WordSignature>; 

public:
    explicit WordSignature() = default;
    explicit WordSignature(const WordSignature& other) = default;
    explicit WordSignature(const std::string& string);

    //Operators functions
    void operator+=(const WordSignature& ws);
    void operator-=(const WordSignature& ws);
    auto operator<=>(const WordSignature& ws) const = default;
    bool isSubsetOf(const WordSignature& ws) const;

    //Getters
    int getCharactersNumber() const;

    //Transformers
    std::string toString() const;

//private:
    std::array<uint8_t, 26> table = {}; //Initialized to 0
};

template<>
struct std::hash<WordSignature>
{
    size_t operator()(const WordSignature& ws) const noexcept
    {
        const uint64_t* p = reinterpret_cast<const uint64_t*>(ws.table.data());
        //26 bytes → 3 chunks (24 bytes) + 2 leftover bytes

        uint64_t h = 0x9e3779b97f4a7c15ull;

        h ^= p[0] * 0xbf58476d1ce4e5b9ull;
        h = (h << 31) | (h >> 33);

        h ^= p[1] * 0x94d049bb133111ebull;
        h = (h << 27) | (h >> 37);

        // Last chunk is 8 bytes, but only 2 contain real data — still safe.
        h ^= p[2] * 0xd6e8feb86659fd93ull;
        h = (h << 33) | (h >> 31);

        return h;
    }
};

#endif
