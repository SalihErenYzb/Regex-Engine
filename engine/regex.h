#ifndef REGEX_H
#define REGEX_H

#define MAXSIZE 1024

#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <fstream>


class Regex {
public:
    char ENDING = '\1';
    char ANYC = '.';

    // Constructor to initialize the pattern
    Regex(const std::string& regex);

    // Function to check if the pattern matches the entire string
    bool match(const std::string& text) const;

    // Function to find all occurrences of the pattern in the text
    std::vector<std::string> findAllMatches(const std::string& text) const;
    void writeGraphToFile(std::string path = "/grapht.txt") const;

private:
    int size;
    std::vector<char> pattern; // holds regex string's chars and extra accept char
    std::vector<int> edges; // holds both black and red edges as indexes to pattern; reds will be negative; every node at most 3 edge
    void fillReachable(std::vector<int>& toDfs, std::unordered_set<int>& reachable,bool& canReach) const;
    // Compile the pattern
    void compilePattern(const std::string& regex);
};

#endif // REGEX_H
