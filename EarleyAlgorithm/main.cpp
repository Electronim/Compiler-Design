#include <bits/stdc++.h>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

class Production {
 public:
    char left;
    string right;
    bool isNullable;

    Production() { }

    Production(char left, string right, bool isNullable) {
        this->left = left;
        this->right = right;
        this->isNullable = isNullable;
    }

    bool operator== (const Production& other) const {
        return this->left == other.left && this->right == other.right;
    }

    bool operator< (const Production& other) const {
        if (this->right == other.right) {
            return this->left < other.left;
        }

        return this->right < other.right;
    }
};

class Grammar {
 private:
    unordered_map < char, vector < Production > > stateHelper;

 public:
    set < char > alphabet; 
    set < char> nonterminals;
    char startSymbol;
    set < Production > productions;

    Grammar() { }

    Grammar(set < char > alphabet, set < char > nonterminals, char startSymbol, set < Production > productions) {
        this->alphabet = alphabet;
        this->nonterminals = nonterminals;
        this->startSymbol = startSymbol;
        this->productions = productions;

        for (auto it: productions) {
            stateHelper[it.left].emplace_back(it);
        }
    }

    vector < Production > getProductions(char nonterminal) {
        auto it = stateHelper.find(nonterminal);
        
        if (it == stateHelper.end()) {
            return vector < Production >();
        }

        return it->second;
    }

    friend ifstream& operator>> (ifstream&, Grammar&);

private:
    set <Production> splitProd(string productionText){
        char nonTerminal;
        string expression;
        set <Production> productions;
        stringstream ls;
        ls.clear();
        ls.str(productionText);
        ls >> nonTerminal;
        ls >> expression;
        while (ls >> expression){
            if (expression == "|")
                continue;
            Production prod;
            prod.left = nonTerminal;
            prod.right = expression;
            prod.isNullable = (expression == "$");
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
        
        for (auto it: newProductions) {
            g.stateHelper[it.left].emplace_back(it);
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

class State {
 private:
    Production production;
    int dotPosition;
    int origin;

 public:
    State(Production production, int dotPosition = 0, int origin = 0) {
        this->production = production;
        this->dotPosition = dotPosition;
        this->origin = origin;
    }

    State(const State& other) {
        this->production = other.production;
        this->dotPosition = other.dotPosition;
        this->origin = other.origin;
    }

    bool operator< (const State& other) const {
        if (this->production == other.production) {
            if (this->dotPosition == other.dotPosition) {
                return this->origin < other.origin;
            }

            return this->dotPosition < other.dotPosition;
        }

        return this->production < other.production;
    }

    bool operator== (const State& other) const {
        return  this->production == other.production &&
                this->dotPosition == other.dotPosition &&
                this->origin == other.origin;
    }

    void setProduction(Production production) {
        this->production = production;
    }

    void setDotPosition(int dotPosition) {
        this->dotPosition = dotPosition;
    }

    void setOrigin(int origin) {
        this->origin = origin;
    }

    Production getProduction() {
        return production;
    }

    int getDotPosition() {
        return dotPosition;
    }

    int getOrigin() {
        return origin;
    }

    bool finished() {
        return dotPosition == production.right.size();
    }

    char nextElement() {
        return production.right[dotPosition];
    }

    bool isElementDotted(char el) {
        return nextElement() == el;
    }

    bool isNonterminalElement(Grammar grammar) {
        char el = nextElement();
        return grammar.nonterminals.find(el) != grammar.nonterminals.end();
    }

    friend ostream& operator<< (ostream&, const State&);
};

ostream& operator<< (ostream& out, const State& state) {
    string rule = state.production.right;
    rule.insert(state.dotPosition, ".");
    out << "[(" << state.production.left << " -> " << rule << "), origin = " << state.origin << "]";

    return out;
}

class EarleyParser {
 private:
    Grammar grammar;
    vector < set < State > > states;
    map < char, bool > isNullable;
    
    void predict(State state, int origin, set < State >& stateContainer) {
        char nonterminalElement = state.nextElement();
        vector < Production > productions = grammar.getProductions(nonterminalElement);

        for (auto it: productions) {
            State newState(it, 0, origin);

            if (states[origin].find(newState) == states[origin].end()) {
                stateContainer.insert(newState);
            }
        }
    }

    void scan(State state, int origin, char terminal) {
        if (origin + 1 >= states.size() || !state.isElementDotted(terminal)) {
            return ;
        }

        State newState(state);
        newState.setDotPosition(state.getDotPosition() + 1);
        states[origin + 1].insert(newState);
    }

    void complete(State state, int origin, set < State >& stateContainer) {
        char stateNonterminal = state.getProduction().left;
        int stateOrigin = state.getOrigin();

        for (auto it: states[stateOrigin]) {
            if (!it.isElementDotted(stateNonterminal)) {
                continue;
            }

            State newState(it);
            newState.setDotPosition(it.getDotPosition() + 1);
            if (states[origin].find(newState) == states[origin].end()) {
                stateContainer.insert(newState);
            }
        }
    }

    void handleLambda(int origin, set < State >& stateContainer) {
        for (auto it: states[origin]) {
            if (it.finished() || (it.nextElement() != '$' && !it.isNonterminalElement(grammar))) {
                continue;
            }

            char nonterminal = it.nextElement();
            if (!isNullable[nonterminal]) {
                continue;
            }

            State newState(it);
            newState.setDotPosition(it.getDotPosition() + 1);
            if (states[origin].find(newState) == states[origin].end()) {
                stateContainer.insert(newState);
            }
        }
    }

    void computeStates(string word) {
        string initial = "";
        initial += grammar.startSymbol;

        State newState(Production('#', initial, 0));
        states[0].insert(newState);

        for (int k = 0; k <= word.size(); ++k) {
            set < State > temp = states[k];
            while (!temp.empty()) {
                set < State > stateContainer;

                for (auto it: temp) {
                    set < State > tempContainer;

                    if (it.finished()) {                            // if is finish state - complete
                        complete(it, k, tempContainer);
                    } else {
                        if (it.isNonterminalElement(grammar)) {     // if is a nonterminal - predict
                            predict(it, k, tempContainer);
                        } else {                                    // if is a terminal - scan
                            scan(it, k, word[k]);
                        }
                    }

                    for (auto it2: tempContainer) {
                        states[k].insert(it2);
                        stateContainer.insert(it2);
                    }
                }

                handleLambda(k, stateContainer);                    // check for nullable productions
                for (auto it2: stateContainer) {
                    states[k].insert(it2);
                }

                temp = stateContainer;
            }

            printState(k);
        }
    }

    void printState(int index) {
        fout << "Print state: ";
        fout << index << ":\n";
        for (auto it: states[index]) {
            fout << it << "\n";
        }

        fout << "\n";
    }

 public:
    EarleyParser() { }

    EarleyParser(Grammar grammar) {
        this->grammar = grammar;

        for (auto it: grammar.productions) {
            if (it.isNullable) {
                this->isNullable[it.left] = true;
            }
        }
    }

    Grammar getGrammar() {
        return grammar;
    }

    string accepts(string word) {
        states.resize(word.size() + 1);
        computeStates(word);

        string initial = "";
        initial += grammar.startSymbol;
        
        State initialState(Production('#', initial, 0), 1, 0);
        string result = word + " is " + 
            (states[word.size()].find(initialState) != states[word.size()].end() ? "ACCEPTED" : "REJECTED");

        states.clear();
        return result;
    }

    void accepts(vector < string > words) {
        for (auto it: words) {
            fout << accepts(it) << "\n\n";
        }
    }
};

int main() {
    Grammar G;
    fin >> G;
    // printGrammar(G);
    
    string word;
    vector < string > words;
    while(fin >> word){
        words.push_back(word);
    }

    EarleyParser earleyParser(G);
    earleyParser.accepts(words);
    return 0;
}    