#ifndef REGEX_H
#define REGEX_H

#define MAXSIZE 1024

#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <codecvt>
#include <locale>

class Regex {
public:
    // Represents literal characters using unicode private use area
    char32_t LITERAL = U'\U000F0000';
    char32_t LITERAL_START = '[';
    char32_t LITERAL_END = ']';
    char32_t ENDING = '\1';
    char32_t ANYC = '.';
    char32_t OR = '|';
    char32_t ZEROORMORE = '*';
    char32_t ONEORMORE = '+';
    char32_t OPEN = '(';
    char32_t CLOSE = ')';
    
    // Constructor takes a normal string
    Regex(const std::string& regex);

    // Functions take and return normal strings
    bool match(const std::string& texts ) const;
    std::vector<std::string> findAllMatches(const std::string& text) const;
    void writeGraphToFile(std::string path = "/grapht.txt") const;

private:
    int size;
    std::vector<char32_t> pattern; // Internal representation uses char32_t
    std::vector<int> edges;

    // maps indexes of literals to their values
    std::unordered_map<int,std::unordered_set<char32_t>> literals;
    // literal_str to be used only in VERBOSE mode
    std::unordered_map<int,std::u32string> literals_str;
    
    void fillReachable(std::vector<int>& toDfs, std::unordered_set<int>& reachable) const;
    void compilePattern(const std::string& regexs);
    
    // Utility functions for internal use
    static std::u32string utf8_to_utf32(const std::string& utf8_string);
    static std::string utf32_to_utf8(const std::u32string& utf32_string);
};

#endif // REGEX_H