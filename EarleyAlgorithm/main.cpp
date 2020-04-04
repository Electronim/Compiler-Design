#include <bits/stdc++.h>

using namespace std;

struct production {
    char r;
    string l;
};

struct Grammar {
    string alphabet; 
    string nonterminals;
    char startSymbol;
    vector < production > productions;

    void read() {

    }
};

class EarleyParser {
 public:
    

};

/*
    input:
        alphabet (sigma) - string
        nonterminals union (N) - string
        start symbol (S) - char
        productions (P) - vector ( char -> string ) S -> aXbX | bXaX | $

        words (W) - vector < string >

    output (set < production>):
        S -> (aa.XbX, 0)
*/

int main() {
    Grammar R;
    R.read();

    EarleyParser ep = new EarleyParser(R);

    for (auto word: words) {
        ep.solve(word);
    } 

    return 0;
}