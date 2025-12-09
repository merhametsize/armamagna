#ifndef WORD_SIGNATURE_H
#define WORD_SIGNATURE_H

#include <cstdint>    //For uint8_t
#include <cstring>   //For std::memcpy
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
    alignas(8) std::array<uint8_t, 26> table = {}; //Initialized to 0
};

template<>
struct std::hash<WordSignature>
{
    size_t operator()(const WordSignature &ws) const noexcept
    {
        const uint8_t* data = ws.table.data();

        uint64_t chunk0, chunk1, chunk2, chunk3;

        //Copy 24 bytes in 3 chunks, then the remaining 2 bytes in chunk3.
        std::memcpy(&chunk0, data + 0,  8);
        std::memcpy(&chunk1, data + 8,  8);
        std::memcpy(&chunk2, data + 16, 8);

        uint16_t tail;
        std::memcpy(&tail, data + 24, 2);
        chunk3 = static_cast<uint64_t>(tail);

        //BULK-MIX HASHING (simple, very fast) =====
        size_t h = 0xcbf29ce484222325ULL; //FNV offset basis

        auto mix = [&](uint64_t v) {
            v ^= v >> 33;
            v *= 0xff51afd7ed558ccdULL;
            v ^= v >> 33;
            v *= 0xc4ceb9fe1a85ec53ULL;
            v ^= v >> 33;
            h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        };

        mix(chunk0);
        mix(chunk1);
        mix(chunk2);
        mix(chunk3);

        return h;
    }
};

#endif
