#include "regex.h"
#include <iostream>

// TODO: Try to find a way to do dfs without using unordered_set for efficiency
// TODO: Make it possible to init giving a map of special characters like (,|,*,+,) etc.
// TODO: Make code cleaner and more readable
// TODO: Add support for [a-z] and [0-9] etc.
// TODO: Add support for ^ and $ for start and end of string
// TODO: Add support for \d, \w, \s etc.
// TODO: Add support for ?, :, or {} etc.
// TODO: Add function to find all matches 
// TODO: Return the matching string which allows indexing for every () in the regex????

#define VERBOSE true

using namespace std;
std::u32string Regex::utf8_to_utf32(const std::string& utf8_string) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(utf8_string);
}

std::string Regex::utf32_to_utf8(const std::u32string& utf32_string) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.to_bytes(utf32_string);
}
void Regex::writeGraphToFile(string path) const {
    // write graph in graphviz format
    ofstream file(path);
    if (!file.is_open()) {
        cerr << "Failed to open " << path << " for writing.\n";
        return;
    }

    
    // Write the initial part of the graph format
    file << "digraph {\n";
    file << "\trankdir=LR\n";
    vector<int> dp(size+1,0);
    // First loop to write the labels for each node
    int i = 0;
    int count = 0;
    string curr = "";
    for (int i = 0; i < size; i++) {
        string utf8_char;
        if (pattern[i] == LITERAL){
            utf8_char = '[' + utf32_to_utf8(literals_str.find(i)->second) + ']';
        }else{
            utf8_char = utf32_to_utf8(std::u32string(1, pattern[i]));
        }
        if (pattern[i] == OR || pattern[i+1] == ZEROORMORE || pattern[i+1] == ONEORMORE || pattern[i] == CLOSE || pattern[i] == OPEN){
            if (curr != ""){
                file << "\t" << count << " [label=\"" << count << ": " << curr << "\"];\n";
                curr = "";
                count++;
            }
            file << "\t" << count << " [label=\"" << count << ": " << utf8_char << "\"];\n";
            dp[i] = count;
            count++;
        }else{
            curr += utf8_char;
            dp[i] = count;
        }
    }
    dp[size] = count;
    // add ending node
    file << "\t" << count << " [label=\"" << count << ": " << "end" << "\"];\n";
    for (int i = 0; i < size; i++) {
        int edge;
        for (int j = 0; j < 3; j++) {
            if (edges[3 * i + j] != MAXSIZE) {
                edge = abs(edges[3 * i + j]);
                if (dp[i] == dp[edge]){
                    continue;
                }
                string color = (edges[3 * i + j] < 0) ? "red" : "black";
                file << "\t" << dp[i] << " -> " << dp[edge] << " [color=" << color << "];\n";
            }
        }
    }
    file << "}\n";
    file.close();
    cout << "Graph has been written to " << path << ".\n";
}
// dfs
void Regex::fillReachable(vector<int>& toDfs, unordered_set<int>& reachable) const {
    reachable.clear();
    while (!toDfs.empty()){
        int curr = toDfs.back();
        toDfs.pop_back();
        if (curr == size){
            reachable.insert(curr);
            return;
        }
        if (reachable.find(curr) != reachable.end()) continue;
        reachable.insert(curr);
        // get all free visitings
        int edge;
        for (int i = 0; i < 3; i++){
            edge = edges[3*curr+i];
            if (edge < 0 && reachable.find(-edge) == reachable.end()){
                toDfs.push_back(-edge);
            }
        }
    }
}


// Constructor to initialize the pattern
Regex::Regex(const std::string& regex) {
    pattern = vector<char32_t>(MAXSIZE);
    edges = vector<int>(MAXSIZE*3,MAXSIZE);
    compilePattern(regex);
}

// Function to compile the pattern
void Regex::compilePattern(const std::string& regexs) {
    // Implement your own regex engine.
    u32string regex = utf8_to_utf32(regexs);
    try {
        if (regex.size() >= MAXSIZE){
            std::cerr << "Regex pattern is too long." << std::endl;
        }
        int literals_size = 0;
        size = regex.size()+2;
        // fill the pattern
        // for simplicity add paranthesis to start and end
        pattern[0] = OPEN;
        for (int i = 1; i < size-1; i++){
            if (regex[i-1] == LITERAL_START){
                pattern[i-literals_size] = LITERAL;
                literals[i-literals_size] = unordered_set<char32_t>();
                i++;
                literals_size++;
                u32string tmp = u32string();
                while (regex[i-1] != ']'){
                    tmp += regex[i-1];
                    literals[i-literals_size].insert(regex[i-1]);
                    i++;
                    literals_size++;
                }
                literals_str[i-literals_size] = tmp;
            }else{
                pattern[i-literals_size] = regex[i-1];
            }
        }
        size -= literals_size;
        pattern[size-1] = CLOSE;
        pattern[size] = ENDING;
        // print regex
        if (VERBOSE){
            cout << "pattern: ";
            for (int i = 0; i < size; i++){
                string utf8_char = utf32_to_utf8(std::u32string(1, pattern[i]));
                if (pattern[i] == LITERAL){
                    cout << "[" << utf32_to_utf8(literals_str[i]) << "]";
                }else{
                    cout << utf8_char;
                }    
            }
            cout << "\n\n";
            // print literals
            // cout << "literals: \n";
            // for (int i = 0; i < size; i++){
            //     if (literals.find(i) != literals.end()){
            //         cout << i << ": ";
            //         for (auto el: literals[i]){
            //             string utf8_char = utf32_to_utf8(std::u32string(1, el));
            //             cout << utf8_char << " ";
            //         }
            //         cout << "\n";
            //     }
            // }
        }
        // fill the edges and pointers
        // there are 3 pointer position for every edge
        // chars,literals and . black points next in first pos
        // (,),*,+ red points next and in first pos also | pointer is in first pos
        // pointers to *,+ are in second pos
        // pointers to back and pointers to next of | are in third
        unordered_set<char> red_points;
        red_points.insert(OPEN);
        red_points.insert(CLOSE);
        red_points.insert(ONEORMORE);
        red_points.insert(ZEROORMORE);
        // stack for ( and | 
        vector<int> st;

        for (int i = 0; i < size; i++){
            // first pos iteration
            if (pattern[i] != OR){
                if (red_points.find(pattern[i]) == red_points.end()){
                    edges[3*i] = i+1;
                    // red pointer
                    if (pattern[i+1] == ZEROORMORE){
                        edges[3*i+1] = -i-1;
                    }
                    // red back pointer
                    if (pattern[i+1] == ONEORMORE || pattern[i+1] == ZEROORMORE){
                        edges[3*i+5] = -i;
                    }
                }else{
                    edges[3*i] = -i-1;
                }
            }
            // stack pushes
            if (pattern[i] == OPEN){
                st.push_back(i);
            }else if (pattern[i] == OR){
                st.push_back(-i);
            }
            // ) end
            if (pattern[i] == CLOSE){
                int last = -1;
                int back;
                while (st.back() < 0){
                    // or end
                    back = -st.back();
                    edges[back*3] = -i;
                    st.pop_back();
                    // need to connect first possibility with next
                    if (last != -1){
                        edges[back*3+5] = -last;
                    }
                    last = back+1;
                }
                if (st.empty() || st.back() < 0){
                    throw std::runtime_error("Invalid regex pattern.");
                }
                back = st.back();
                st.pop_back();
                // if there was or than connect it to last
                if (last != -1){
                    edges[back*3+5] = -last;
                }
                // check for * and +
                if (pattern[i+1] == ZEROORMORE || pattern[i+1] == ONEORMORE){
                    edges[i*3+5] = -back;
                    if (pattern[i+1] == ZEROORMORE){
                        edges[back*3+2] = -i-1;
                    }
                }
            }
        }

        // for verbose
        if (VERBOSE){
            for (int i = 0; i < size; i++){
                // check for literal
                string utf8_char;
                if (pattern[i] == LITERAL){
                    utf8_char = '[' + utf32_to_utf8(literals_str[i]) + ']';
                }else{
                    utf8_char = utf32_to_utf8(std::u32string(1, pattern[i]));
                }
                cout << i << "th state: " << utf8_char << "\n";
                cout << "edges: - ";
                int edge;
                for (int j = 0; j < 3; j++){
                    if (edges[3*i+j] != MAXSIZE){
                        if (edges[3*i+j] < 0){
                            cout << "red ";
                        }
                        edge = abs(edges[3*i+j]);
                        utf8_char = utf32_to_utf8(std::u32string(1, pattern[edge]));
                        cout << "pttrn[" << edge << "] = " << utf8_char << " - ";
                    }                        
                }
                cout << "\n\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Invalid regex pattern: " << e.what() << std::endl;
    }
}

// Function to check if the pattern matches the entire string
bool Regex::match(const std::string& texts) const {
    u32string text = utf8_to_utf32(texts);
    vector<int> toDfs;
    toDfs.push_back(0);
    unordered_set<int> reachable;
    bool canReach = false;
    for (int i = 0; i < text.size(); i++){
        // dfs
        fillReachable(toDfs,reachable);
        // if we can get to curr char good
        for (int el: reachable){
            if (el == size){
                continue;
            }
            // cout << "reachable at " << i << ": " << pattern[el] << "\n";
            if (pattern[el] == LITERAL){
                auto it = literals.find(el);
                if (it == literals.end()){
                    throw std::runtime_error("Invalid regex pattern.");
                }
                if (it->second.find(text[i]) == it->second.end()){
                    continue;
                }else{
                    // add as if it has been added by doing el+1
                    toDfs.push_back(el+1);
                }
            }
            else if (pattern[el] == text[i] || pattern[el] == ANYC){
                // add as if it has been added by doing el+1
                toDfs.push_back(el+1);
            }
            if (VERBOSE && (pattern[el] == text[i] || pattern[el] == ANYC)){
                string utf8_char;
                if (pattern[el+1] == LITERAL){
                    utf8_char = '[' + utf32_to_utf8(literals_str.find(el+1)->second) + ']';
                }else{
                    utf8_char = utf32_to_utf8(std::u32string(1, pattern[el+1]));
                }
                cout << "From " << text[i] << ", " << utf8_char << " can be reached.\n"; 
            }
        }
        // if we cant reach return false
        if (toDfs.empty()){
            return false;
        }
    }
   // if we can reach end return true
    fillReachable(toDfs,reachable);
    return reachable.find(size) != reachable.end();
}

// Function to find all occurrences of the pattern in the text
std::vector<std::string> Regex::findAllMatches(const std::string& text) const {
    return vector<string>(1,"");
}


// usage
int main(){
    // write very very long regex pattern
    string regex = "ampu[abcd]rn";
    Regex reg(regex);
    cout << reg.match("ampucrn") << "\n";
    reg.writeGraphToFile("../grapht.dot");
    return 0;
}
