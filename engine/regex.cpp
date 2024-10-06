#include "regex.h"
#include <iostream>

// TODO: Try to find a way to do dfs without using unordered_set for efficiency
// TODO: Make it possible to run with UTF-8 characters
// TODO: Make it possible to init giving a map of special characters like (,|,*,+,) etc.
// TODO: Make code cleaner and more readable
// TODO: Add support for [a-z] and [0-9] etc.
// TODO: Add support for ^ and $ for start and end of string
// TODO: Add function to find all matches 
// TODO: Return the matching string which allows indexing for every () in the regex????

#define VERBOSE false

using namespace std;
// void Regex::writeGraphToFile(string path) const {
//     // write graph in graphviz format
//     ofstream file(path);
//     if (!file.is_open()) {
//         cerr << "Failed to open " << path << " for writing.\n";
//         return;
//     }
    
//     // Write the initial part of the graph format
//     file << "digraph {\n";
//     file << "\trankdir=LR\n";

//     // First loop to write the labels for each node
//     for (int i = 0; i < size; i++) {
//         file << "\t" << i << " [label=\"" << i << ": " << pattern[i] << "\"];\n";
//     }
//     // add ending node
//     file << "\t" << size << " [label=\"" << size << ": " << "end" << "\"];\n";
//     for (int i = 0; i < size; i++) {
//         int edge;
//         for (int j = 0; j < 3; j++) {
//             if (edges[3 * i + j] != MAXSIZE) {
//                 edge = abs(edges[3 * i + j]);
//                 string color = (edges[3 * i + j] < 0) ? "red" : "black";
//                 file << "\t" << i << " -> " << edge << " [color=" << color << "];\n";
//             }
//         }
//     }
//     file << "}\n";
//     file.close();
//     cout << "Graph has been written to " << path << ".\n";
// }
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
        if (pattern[i] == '|' || pattern[i] == '*' || pattern[i] == '+' || pattern[i] == ')' || pattern[i] == '('){
            if (curr != ""){
                file << "\t" << count << " [label=\"" << count << ": " << curr << "\"];\n";
                curr = "";
                count++;
                
            }
            file << "\t" << count << " [label=\"" << count << ": " << pattern[i] << "\"];\n";
            dp[i] = count;
            count++;
        }else{
            curr += pattern[i];
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
void Regex::fillReachable(vector<int>& toDfs, unordered_set<int>& reachable,bool& canReach) const {
    reachable.clear();
    while (!toDfs.empty()){
        int curr = toDfs.back();
        toDfs.pop_back();
        if (curr == size){
            canReach = true;
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
    pattern = vector<char>(MAXSIZE);
    edges = vector<int>(MAXSIZE*3,MAXSIZE);
    compilePattern(regex);
}

// Function to compile the pattern
void Regex::compilePattern(const std::string& regex) {
    // Implement your own regex engine.
    try {
        if (regex.size() >= MAXSIZE){
            std::cerr << "Regex pattern is too long." << std::endl;
        }
        size = regex.size()+2;
        // fill the pattern
        // for simplicity add paranthesis to start and end
        pattern[0] = '(';
        pattern[size-1] = ')';
        for (int i = 1; i < size-1; i++){
            pattern[i] = regex[i-1];
        }
        pattern[size] = ENDING;
        // print regex
        if (VERBOSE){
            cout << "pattern: ";
            for (int i = 0; i < size; i++){
                cout << pattern[i];
            }
            cout << "\n\n";
        }
        // fill the edges and pointers
        // there are 3 pointer position for every edge
        // chars,literals and . black points next in first pos
        // (,),*,+ red points next and in first pos also | pointer is in first pos
        // pointers to *,+ are in second pos
        // pointers to back and pointers to next of | are in third
        unordered_set<char> red_points;
        red_points.insert('(');
        red_points.insert(')');
        red_points.insert('+');
        red_points.insert('*');
        // stack for ( and | 
        vector<int> st;

        for (int i = 0; i < size; i++){
            // first pos iteration
            if (pattern[i] != '|'){
                if (red_points.find(pattern[i]) == red_points.end()){
                    edges[3*i] = i+1;
                    // red pointer
                    if (pattern[i+1] == '*'){
                        edges[3*i+1] = -i-1;
                    }
                    // red back pointer
                    if (pattern[i+1] == '+' || pattern[i+1] == '*'){
                        edges[3*i+5] = -i;
                    }
                }else{
                    edges[3*i] = -i-1;
                }
            }
            // stack pushes
            if (pattern[i] == '('){
                st.push_back(i);
            }else if (pattern[i] == '|'){
                st.push_back(-i);
            }
            // ) end
            if (pattern[i] == ')'){
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
                if (pattern[i+1] == '*' || pattern[i+1] == '+'){
                    edges[i*3+5] = -back;
                    if (pattern[i+1] == '*'){
                        edges[back*3+2] = -i-1;
                    }
                }
            }
        }

        // for verbose
        if (VERBOSE){
            for (int i = 0; i < size; i++){
                cout << i << "th state: " << pattern[i] << "\n";
                cout << "edges: - ";
                int edge;
                for (int j = 0; j < 3; j++){
                    if (edges[3*i+j] != MAXSIZE){
                        if (edges[3*i+j] < 0){
                            cout << "red ";
                        }
                        edge = abs(edges[3*i+j]);
                        cout << "pttrn[" << edge << "] = " << pattern[edge] << " - ";
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
bool Regex::match(const std::string& text) const {
    vector<int> toDfs;
    toDfs.push_back(0);
    unordered_set<int> reachable;
    bool canReach = false;
    for (int i = 0; i < text.size(); i++){
        // dfs
        fillReachable(toDfs,reachable,canReach);
        // if we can get to curr char good
        for (auto el: reachable){
            // cout << "reachable at " << i << ": " << pattern[el] << "\n";
            if (pattern[el] == text[i] || pattern[el] == ANYC){
                // add as if it has been added by doing el+1
                toDfs.push_back(el+1);
            }
            if (VERBOSE && (pattern[el] == text[i] || pattern[el] == ANYC)){
                cout << "From " << text[i] << ", " << pattern[el+1] << " can be reached.\n"; 
            }
        }
        // if we cant reach return false
        if (toDfs.empty()){
            return false;
        }
    }
    return canReach;
}

// Function to find all occurrences of the pattern in the text
std::vector<std::string> Regex::findAllMatches(const std::string& text) const {
    return vector<string>(1,"");
}


// usage
int main(){
    // write very very long regex pattern
    string regex = "(a+b+c+d+|ocherif|samosamo)*yarrakye";
    Regex reg(regex);
    cout << reg.match("ocherifsamosamoaaaaabbcdddocherif") << "\n";
    reg.writeGraphToFile("../grapht.dot");
    return 0;
}
