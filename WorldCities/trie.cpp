#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

struct TrieNode {
    bool isEndOfWord;
    unordered_map<char, TrieNode*> children;

    TrieNode() : isEndOfWord(false) {}
};

class NameTrie {
private:
    TrieNode* root;

public:
    NameTrie() {
        root = new TrieNode();
    }

    void insert(const string& name) {
        TrieNode* node = root;
        for (char c : name) {
            c = tolower(c); // Case-insensitive
            if (node->children.count(c) == 0)
                node->children[c] = new TrieNode();
            node = node->children[c];
        }
        node->isEndOfWord = true;
    }

    bool search(const string& name) {
        TrieNode* node = root;
        for (char c : name) {
            c = tolower(c);
            if (node->children.count(c) == 0)
                return false;
            node = node->children[c];
        }
        return node->isEndOfWord;
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


int main() {
    NameTrie trie;

    // Insert names
    trie.insert("Saba");
    trie.insert("Sam");
    trie.insert("David");
    trie.insert("Dave");

    trie.printTrie();

    return 0;
}
