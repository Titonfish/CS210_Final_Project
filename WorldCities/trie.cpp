#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
using namespace std;

struct TrieNode {
    bool isEndOfWord;
    unordered_map<char, TrieNode*> children;
    vector<vector<string>> citiesData{};

    TrieNode() : isEndOfWord(false) {}
};

class CityTrie {
private:
    TrieNode* root;

public:
    CityTrie() {
        root = new TrieNode();
    }

    void insert(const string& name, const vector<string>& cityData) {
        TrieNode* node = root;
        for (char c : name) {
            c = tolower(c); // Case-insensitive
            if (node->children.count(c) == 0)
                node->children[c] = new TrieNode();
            node = node->children[c];
        }
        node->isEndOfWord = true;
        node->citiesData.push_back(cityData);
    }

    vector<vector<string>> search(const string& name) {
        TrieNode* node = root;
        for (char c : name) {
            c = tolower(c);
            if (node->children.count(c) == 0)
                return vector<vector<string>>();
            node = node->children[c];
        }
        return node->citiesData;
    }

    void printTrie(TrieNode* node = nullptr, string prefix = "", string indent = "") {
        if (!node) node = root;
        if (node->isEndOfWord)
            cout << indent << "'" << prefix << "' (END)" << endl;
        for (auto& [ch, child] : node->children) {
            printTrie(child, prefix + ch, indent + "  ");
        }
    }
};