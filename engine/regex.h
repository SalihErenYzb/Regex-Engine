#ifndef REGEX_H
#define REGEX_H

#define MAXSIZE 1024

#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <fstream>
#include <codecvt>
#include <locale>

class Regex {
public:
    // These can remain as char since they're ASCII
    char ENDING = '\1';
    char ANYC = '.';
    char OR = '|';
    char ZEROORMORE = '*';
    char ONEORMORE = '+';
    char OPEN = '(';
    char CLOSE = ')';
    
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
    
    void fillReachable(std::vector<int>& toDfs, std::unordered_set<int>& reachable) const;
    void compilePattern(const std::string& regexs);
    
    // Utility functions for internal use
    static std::u32string utf8_to_utf32(const std::string& utf8_string);
    static std::string utf32_to_utf8(const std::u32string& utf32_string);
};

#endif // REGEX_H