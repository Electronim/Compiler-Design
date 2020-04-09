#include <bits/stdc++.h>

using namespace std;

ifstream fin("input.txt");

class Production {
 public:
    char left;
    string right;

    Production() { }

    Production(char left, string right) {
        this->left = left;
        this->right = right;
    }

    bool operator == (const Production& other) const {
        return this->left == other.left && this->right == other.right;
    }

    bool operator < (const Production& other) const {
        return this->right < other.right;
    }
};

class Grammar {
 public:
    set< char > alphabet; 
    set < char> nonterminals;
    char startSymbol;
    set < Production > productions;

    friend ifstream& operator>> (ifstream&, Grammar&);

private:
    set <Production> splitProd(string productionText){
        char nonTerminal;
        string expreession;
        set <Production> productions;
        stringstream ls;
        ls.clear();
        ls.str(productionText);
        ls >> nonTerminal;
        ls >> expreession;
        while (ls >> expreession){
            if (expreession == "|")
                continue;
            Production prod;
            prod.left = nonTerminal;
            prod.right = expreession;
            productions.insert(prod);
        }
        return productions;
    }
};

ifstream& operator>> (ifstream& fin, Grammar& g){
    string alpha, nonTerms, productionText;
    int numberOfProductions;
    fin >> alpha >> nonTerms >> g.startSymbol >> numberOfProductions;
    fin.ignore();
    for (auto a: alpha)
        g.alphabet.insert(a);
    for (auto n: nonTerms){
        g.nonterminals.insert(n);
    }
    for (int i = 0; i < numberOfProductions; i++){
        getline(fin, productionText);
        set < Production > newProductions = g.splitProd(productionText);
        for (auto p: newProductions){
            g.productions.insert(p);
        }
    }
}

void printGrammar(Grammar g){
    for (auto a: g.alphabet){
        cout << a << " ";
    }
    cout << endl;
    for (auto n: g.nonterminals){
        cout << n << " ";
    }
    cout << endl;
    cout << g.startSymbol << endl;
    for (auto p: g.productions){
        cout << p.left << " -> " << p.right << endl;
    }
}

class EarleyParser {
 public:
    

};

int main() {
    Grammar R;
    fin >> R;
    printGrammar(R);
    
    string word;
    vector < string > words;
    while(fin >> word){
        words.push_back(word);
    }

    return 0;
}