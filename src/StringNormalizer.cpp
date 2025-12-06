#include <string>
#include <expected>
#include <unordered_map>
#include <cctype>

namespace StringNormalizer
{
    // Map of UTF-8 sequences to ASCII equivalents
    static const std::unordered_map<std::string, std::string> utf82ASCII = 
    {
        // ------------------------------------------------------------
        // LATIN-1 SUPPLEMENT (U+00C0–U+00FF)
        // ------------------------------------------------------------

        // Uppercase A
        {"\xC3\x80","a"}, {"\xC3\x81","a"}, {"\xC3\x82","a"}, {"\xC3\x83","a"},
        {"\xC3\x84","a"}, {"\xC3\x85","a"},
        // AE
        {"\xC3\x86","ae"},
        {"\xC3\x87","c"},
        {"\xC3\x88","e"}, {"\xC3\x89","e"}, {"\xC3\x8A","e"}, {"\xC3\x8B","e"},
        {"\xC3\x8C","i"}, {"\xC3\x8D","i"}, {"\xC3\x8E","i"}, {"\xC3\x8F","i"},
        {"\xC3\x90","d"},
        {"\xC3\x91","n"},
        {"\xC3\x92","o"}, {"\xC3\x93","o"}, {"\xC3\x94","o"}, {"\xC3\x95","o"}, {"\xC3\x96","o"},
        {"\xC3\x98","o"},
        {"\xC3\x99","u"}, {"\xC3\x9A","u"}, {"\xC3\x9B","u"}, {"\xC3\x9C","u"},
        {"\xC3\x9D","y"},
        {"\xC3\x9E","th"},
        {"\xC3\x9F","ss"},

        // Lowercase
        {"\xC3\xA0","a"}, {"\xC3\xA1","a"}, {"\xC3\xA2","a"}, {"\xC3\xA3","a"},
        {"\xC3\xA4","a"}, {"\xC3\xA5","a"},
        {"\xC3\xA6","ae"},
        {"\xC3\xA7","c"},
        {"\xC3\xA8","e"}, {"\xC3\xA9","e"}, {"\xC3\xAA","e"}, {"\xC3\xAB","e"},
        {"\xC3\xAC","i"}, {"\xC3\xAD","i"}, {"\xC3\xAE","i"}, {"\xC3\xAF","i"},
        {"\xC3\xB0","d"},
        {"\xC3\xB1","n"},
        {"\xC3\xB2","o"}, {"\xC3\xB3","o"}, {"\xC3\xB4","o"}, {"\xC3\xB5","o"}, {"\xC3\xB6","o"},
        {"\xC3\xB8","o"},
        {"\xC3\xB9","u"}, {"\xC3\xBA","u"}, {"\xC3\xBB","u"}, {"\xC3\xBC","u"},
        {"\xC3\xBD","y"}, {"\xC3\xBF","y"},
        {"\xC3\xBE","th"},

        // ------------------------------------------------------------
        // LATIN EXTENDED-A (U+0100–U+017F)
        // ------------------------------------------------------------

        // A with macron/ogonek/etc
        {"\xC4\x80","a"}, {"\xC4\x81","a"},
        {"\xC4\x82","a"}, {"\xC4\x83","a"},
        {"\xC4\x84","a"}, {"\xC4\x85","a"},

        // C with caron/acute/dot
        {"\xC4\x86","c"}, {"\xC4\x87","c"},
        {"\xC4\x88","c"}, {"\xC4\x89","c"},
        {"\xC4\x8A","c"}, {"\xC4\x8B","c"},
        {"\xC4\x8C","c"}, {"\xC4\x8D","c"},

        // D with caron/stroke
        {"\xC4\x8E","d"}, {"\xC4\x8F","d"},
        {"\xC4\x90","d"}, {"\xC4\x91","d"},

        // E variants
        {"\xC4\x92","e"}, {"\xC4\x93","e"},
        {"\xC4\x94","e"}, {"\xC4\x95","e"},
        {"\xC4\x96","e"}, {"\xC4\x97","e"},
        {"\xC4\x98","e"}, {"\xC4\x99","e"},
        {"\xC4\x9A","e"}, {"\xC4\x9B","e"},

        // G variants
        {"\xC4\x9C","g"}, {"\xC4\x9D","g"},
        {"\xC4\x9E","g"}, {"\xC4\x9F","g"},
        {"\xC4\xA0","g"}, {"\xC4\xA1","g"},
        {"\xC4\xA2","g"}, {"\xC4\xA3","g"},

        // H variants
        {"\xC4\xA4","h"}, {"\xC4\xA5","h"},
        {"\xC4\xA6","h"}, {"\xC4\xA7","h"},

        // I variants
        {"\xC4\xA8","i"}, {"\xC4\xA9","i"},
        {"\xC4\xAA","i"}, {"\xC4\xAB","i"},
        {"\xC4\xAC","i"}, {"\xC4\xAD","i"},
        {"\xC4\xAE","i"}, {"\xC4\xAF","i"},
        {"\xC4\xB0","i"}, {"\xC4\xB1","i"},  // i dotless → i

        // J
        {"\xC4\xB2","j"}, {"\xC4\xB3","j"},

        // K
        {"\xC4\xB4","k"}, {"\xC4\xB5","k"},
        {"\xC4\xB6","k"}, {"\xC4\xB7","k"},

        // L + crossed L etc.
        {"\xC4\xB8","l"}, {"\xC4\xB9","l"},
        {"\xC4\xBA","l"}, {"\xC4\xBB","l"},
        {"\xC4\xBC","l"}, {"\xC4\xBD","l"},
        {"\xC4\xBE","l"}, {"\xC5\x81","l"},
        {"\xC5\x82","l"},

        // N
        {"\xC5\x83","n"}, {"\xC5\x84","n"},
        {"\xC5\x85","n"}, {"\xC5\x86","n"},
        {"\xC5\x87","n"}, {"\xC5\x88","n"},

        // O extended
        {"\xC5\x8C","o"}, {"\xC5\x8D","o"},
        {"\xC5\x8E","o"}, {"\xC5\x8F","o"},
        {"\xC5\x90","o"}, {"\xC5\x91","o"},

        // R
        {"\xC5\x94","r"}, {"\xC5\x95","r"},
        {"\xC5\x96","r"}, {"\xC5\x97","r"},
        {"\xC5\x98","r"}, {"\xC5\x99","r"},

        // S
        {"\xC5\x9A","s"}, {"\xC5\x9B","s"},
        {"\xC5\x9C","s"}, {"\xC5\x9D","s"},
        {"\xC5\x9E","s"}, {"\xC5\x9F","s"},
        {"\xC5\xA0","s"}, {"\xC5\xA1","s"},

        // T
        {"\xC5\xA2","t"}, {"\xC5\xA3","t"},
        {"\xC5\xA4","t"}, {"\xC5\xA5","t"},

        // U
        {"\xC5\xA6","u"}, {"\xC5\xA7","u"},
        {"\xC5\xA8","u"}, {"\xC5\xA9","u"},
        {"\xC5\xAA","u"}, {"\xC5\xAB","u"},
        {"\xC5\xAC","u"}, {"\xC5\xAD","u"},
        {"\xC5\xAE","u"}, {"\xC5\xAF","u"},
        {"\xC5\xB0","u"}, {"\xC5\xB1","u"},

        // W
        {"\xC5\xB2","w"}, {"\xC5\xB3","w"},

        // Y
        {"\xC5\xB4","y"}, {"\xC5\xB5","y"},
        {"\xC5\xB6","y"}, {"\xC5\xB7","y"},

        // Z
        {"\xC5\xB8","y"},
        {"\xC5\xB9","z"}, {"\xC5\xBA","z"},
        {"\xC5\xBB","z"}, {"\xC5\xBC","z"},
        {"\xC5\xBD","z"}, {"\xC5\xBE","z"},

        // ------------------------------------------------------------
        // LATIN EXTENDED-B (Important letters only)
        // ------------------------------------------------------------

        {"\xC6\x80","b"}, {"\xC6\x81","b"},
        {"\xC6\x82","b"}, {"\xC6\x83","b"},
        {"\xC6\x84","c"}, {"\xC6\x85","c"},
        {"\xC6\x86","d"}, {"\xC6\x87","d"},
        {"\xC6\x88","d"}, {"\xC6\x89","e"},
        {"\xC6\x8A","f"}, {"\xC6\x8B","f"},
        {"\xC6\x8C","g"}, {"\xC6\x8D","g"},
        {"\xC6\x8E","h"}, {"\xC6\x8F","i"},

        // ------------------------------------------------------------
        // LIGATURES
        // ------------------------------------------------------------

        {"\xC5\x92","oe"},    // Œ
        {"\xC5\x93","oe"},    // œ
        {"\xC3\x9F","ss"},    // ß

        // ------------------------------------------------------------
        // COMBINING DIACRITICS (U+0300–U+036F)
        // → just remove them (strip accents)
        // ------------------------------------------------------------

        {"\xCC\x80",""}, {"\xCC\x81",""}, {"\xCC\x82",""}, {"\xCC\x83",""},
        {"\xCC\x84",""}, {"\xCC\x85",""}, {"\xCC\x86",""}, {"\xCC\x87",""},
        {"\xCC\x88",""}, {"\xCC\x89",""}, {"\xCC\x8A",""}, {"\xCC\x8B",""},
        {"\xCC\x8C",""}, {"\xCC\x8D",""}, {"\xCC\x8E",""}, {"\xCC\x8F",""},
        {"\xCC\x90",""}, {"\xCC\x91",""}, {"\xCC\x92",""}, {"\xCC\x93",""},
        {"\xCC\x94",""}, {"\xCC\x95",""}, {"\xCC\x96",""}, {"\xCC\x97",""},
        {"\xCC\x98",""}, {"\xCC\x99",""}, {"\xCC\x9A",""}, {"\xCC\x9B",""},
        {"\xCC\x9C",""}, {"\xCC\x9D",""}, {"\xCC\x9E",""}, {"\xCC\x9F",""},

        // ------------------------------------------------------------
        // ROMANIAN LETTERS (Latin Extended-B)
        // ------------------------------------------------------------

        // S with comma below
        {"\xC8\x98", "s"},   // Ș
        {"\xC8\x99", "s"},   // ș

        // T with comma below
        {"\xC8\x9A", "t"},   // Ț
        {"\xC8\x9B", "t"},   // ț

    };


    // UTF-8 safe function to decode a codepoint and advance index
    static bool decodeUTF8(const std::string& s, size_t& i, std::string& out) 
    {
        unsigned char c = static_cast<unsigned char>(s[i]);
        size_t bytes = 0;
        
        if(c <= 0x7F) 
        {        // ASCII
            out = s.substr(i,1);
            i += 1;
            return true;
        } 
        else if ((c & 0xE0) == 0xC0) bytes = 2;    // 2-byte sequence
        else if ((c & 0xF0) == 0xE0) bytes = 3;   // 3-byte sequence
        else if ((c & 0xF8) == 0xF0) bytes = 4;  // 4-byte sequence
        else return false;                      // invalid

        if(i + bytes > s.size()) return false;

        out = s.substr(i, bytes);
        i += bytes;
        return true;
    }

    // Normalize to ASCII alphabetic sequence
    auto normalize(const std::string &s) -> std::expected<std::string, std::string>
    {
        std::string result;
        size_t i = 0;

        while(i < s.size()) 
        {
            std::string cp;
            if(!decodeUTF8(s, i, cp)) {return std::unexpected("Invalid UTF-8 in string \"" + s + "\"");}

            if(cp.size() == 1) 
            {
                char c = cp[0];
                if(isalpha(static_cast<unsigned char>(c))) result += static_cast<char>(tolower(c));
            } 
            else 
            {
                auto it = utf82ASCII.find(cp);
                if (it != utf82ASCII.end()) {result += it->second;} 
                else 
                {
                    // Ignore unknown characters or throw
                    // result += ""; // ignore
                    return std::unexpected("Cannot normalize character in string \"" + s + "\"");
                }
            }
        }

        return result;

    }
} //Namespace StringNormalizer

// Example usage
/*
#include <iostream>
int main() {
std::string s = "per-ché Æßœ çñ ü";
auto norm = normalize(s);
if(norm) std::cout << *norm << "\n"; // prints: percheaessoe cn u
}
*/
