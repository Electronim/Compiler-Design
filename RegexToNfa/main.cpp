#include <bits/stdc++.h>

using namespace std;

ifstream fin("test.in");
ofstream fout("test.out");
const int NMAX = 205;
const char lambda = '$';

class ExprToENFA{
    struct my {
        char ch;
        int count, pos;

        int startState;
        int finishState;
    };

    struct Edge {
        int source;
        int dest;
        char value;

        Edge(int _source, int _dest, char _value) {
            source = _source;
            dest = _dest;
            value = _value;
        }
    };

    bool isPrefixForm;
    string normalExpr, alphaInput;
    my expr[NMAX], root;
    queue < my > Polish;
    vector < my > Tree[NMAX];
    map < int , char > Map;
    set < char > Alpha;

    int states = 0;
    vector < Edge > edges;

public:
    ExprToENFA(string _alphaInput = "", string _normalExpr = "", bool _isPrefixForm = false) : alphaInput(_alphaInput), isPrefixForm(_isPrefixForm) {
        if (_isPrefixForm) {
            normalExpr = preToInfix(_normalExpr) + "#";
        } else {
            normalExpr = "(" + _normalExpr + ")#";
        }
    }
    
    ExprToENFA(const ExprToENFA& other){
        this->alphaInput = other.alphaInput;
        this->normalExpr = other.normalExpr;
        this->isPrefixForm = other.isPrefixForm;
    }

    // preprocessing expression
    void setValue(){
        int cnt = 0;

        for (int i = 0; i < normalExpr.size(); i++){
            if (isOk(normalExpr[i])){
                expr[i] = {normalExpr[i], ++cnt, i};
                Map[cnt] = normalExpr[i];
                
                if (normalExpr[i] != '#') Alpha.insert(normalExpr[i]);
            } else {
                expr[i] = {normalExpr[i], -1, i};
            }
        }
    }

    void addEdge(int source, int dest, char value = lambda) {
        edges.emplace_back(Edge(source, dest, value));
    }

    void getEdges() {
        for (auto it : edges) {
            fout << it.source << " -> " << it.dest << "; value: " << it.value << ";\n"; 
        }
    }

    string getExpr(){
        return normalExpr;
    }

    void getValues(){
        for (int i = 0; i < normalExpr.size(); i++)
            fout << expr[i].ch << " "  << expr[i].count << " " << expr[i].pos << "\n";
    }

    // prints the regular expression in reverse Polish notation
    void getPolish(){
        while (Polish.size()){
            cout << Polish.front().ch;
            Polish.pop();
        }
        cout << "\n";
    }

    my getRoot(){
        fout << root.ch << " " << root.count << " " << root.pos << "\n";
    }

    // prints the tree
    void getTree(my x){
        cout << x.pos << " ( " << x.ch << " , " << x.count << " )\n";
        
        if (Tree[x.pos].size()){
            cout << "left " << x.pos << "( " << x.ch << " , " << x.count << " ) = ";
            getTree(Tree[x.pos][0]);
        }
        
        if (Tree[x.pos].size() > 1){
            cout << "right " << x.pos << "( " << x.ch << " , " << x.count << " ) = ";
            getTree(Tree[x.pos][1]);
        }
    }

    bool isOk(char c){
        return !(c == '|' || c == '*' || c == '(' || c == ')'  || c == '.');
    }

    // adds concatination sign "." needed for building the syntax tree
    void addConcat(){
        string res = "";

        for (int i = 0; i < normalExpr.size(); i++){
            res += normalExpr[i];

            if (i < normalExpr.size() - 1 && (normalExpr[i + 1] != '*' && normalExpr[i + 1] != ')' && normalExpr[i + 1] != '|') && 
                (normalExpr[i] != '|' && normalExpr[i] != '('))
                res += '.';

        }

        normalExpr = res;
    }

    string preToInfix(string expr) {
        stack < string > St;

        int sz = expr.size();
        for (int i = sz - 1; i >= 0; --i) {
            if (expr[i] == '.' || expr[i] == '|') {
                string op1 = St.top(); St.pop();
                string op2 = St.top(); St.pop();

                string temp = "(" + op1 + expr[i] + op2 + ")";
                St.push(temp);
            } else if (expr[i] == '*') {
                string op = St.top(); St.pop();

                string temp = "(" + op + expr[i] + ")";
                St.push(temp);
            } else {
                St.push(string(1, expr[i]));
            }
        }

        string infix = St.top(), result = "";

        // remove concat for further computation
        for (auto it : infix) {
            if (it == '.') continue;

            result += it;
        }

        return result;
    }

    // builds the postfix notation (named reverse Polish notation) which helps to build the syntax tree
    void toPolish(){
        stack < my > Op;
        
        for (int i = 0; i < normalExpr.size(); i++){
            if (normalExpr[i] != '|' && normalExpr[i] != '.' && normalExpr[i] != '(' && normalExpr[i] != ')')
                Polish.push(expr[i]);

            if (normalExpr[i] == '(') Op.push(expr[i]);

            if (normalExpr[i] == ')'){
                while (Op.top().ch != '('){
                    Polish.push(Op.top());
                    Op.pop();
                }

                Op.pop();
            }

            if (normalExpr[i] == '|'){
                while (Op.size() && (Op.top().ch == '.' || Op.top().ch == '|')){
                    Polish.push(Op.top());
                    Op.pop();
                }

                Op.push(expr[i]);
            }

            if (normalExpr[i] == '.'){
                while (Op.size() && Op.top().ch == '.'){
                    Polish.push(Op.top());
                    Op.pop();
                }

                Op.push(expr[i]);
            }

        }

        while(Op.size()){
            Polish.push(Op.top());
            Op.pop();
        }

    }

    // builds the syntax tree from reverse Polish notation
    void toSyntaxTree(){
        stack < my > Eval;

        while (Polish.size()){
            my elem = Polish.front(); Polish.pop();
            if (isOk(elem.ch)){
                Eval.push(elem);
            }

            if (elem.ch == '|'){
                Tree[elem.pos].resize(2);
                Tree[elem.pos][0] = Eval.top(); Eval.pop();
                Tree[elem.pos][1] = Eval.top(); Eval.pop();
                Eval.push(elem);
            }

            if (elem.ch == '*'){
                Tree[elem.pos].resize(1);
                Tree[elem.pos][0] = Eval.top(); Eval.pop();
                Eval.push(elem);
            }

            if (elem.ch == '.'){
                Tree[elem.pos].resize(2);
                Tree[elem.pos][0] = Eval.top(); Eval.pop();
                Tree[elem.pos][1] = Eval.top(); Eval.pop();
                Eval.push(elem);
            }
        }

        root = Eval.top(); Eval.pop();

        // omit de # sign
        if (Tree[root.pos][0].ch != '#') root = Tree[root.pos][0];
        else root = Tree[root.pos][1];
    }

    // normalizes tree to have the "correct order"
    void normalizeTree(){
        for (int i = 0; i < normalExpr.size(); i++){
            if (Tree[i].size() > 1) swap(Tree[i][0], Tree[i][1]);
        }
    }

    // computes the lambda-NFA graph
    void computeENFA(my &elem, int startState) {
        int finishState;

        if (isOk(elem.ch)) {
            finishState = ++states;

            elem.startState = startState;
            elem.finishState = finishState;

            addEdge(startState, finishState, elem.ch);
            return ;
        }

        my left = Tree[elem.pos][0], right;
        if (Tree[elem.pos].size() > 0) {
            right = Tree[elem.pos][1];
        }

        switch (elem.ch)
        {
        case '.': 
            computeENFA(left, startState);
            computeENFA(right, left.finishState);

            elem.startState = left.startState;
            elem.finishState = right.finishState;
            break;

        case '|':
            computeENFA(left, ++states);
            computeENFA(right, ++states);

            finishState = ++states;

            addEdge(startState, left.startState);
            addEdge(startState, right.startState);
            addEdge(left.finishState, finishState);
            addEdge(right.finishState, finishState);

            elem.startState = startState;
            elem.finishState = finishState;
            break;

        case '*': 
            computeENFA(left, ++states);

            finishState = ++states;

            addEdge(startState, left.startState);
            addEdge(left.finishState, left.startState);
            addEdge(left.finishState, finishState);
            addEdge(startState, finishState);

            elem.startState = startState;
            elem.finishState = finishState;
            break;
        
        default:
            break;
        }
    }

    void getStartFinishStates() {
        fout << "S: " << root.startState << "; F: " << root.finishState << ".";
    }

    // checks if alphabet is correct
    void checkIfValidInput() {
        vector < char > a;
        for (auto it : Alpha) {
            a.push_back(it);
        }

        sort(a.begin(), a.end());
        sort(alphaInput.begin(), alphaInput.end());
        if (!includes(alphaInput.begin(), alphaInput.end(), a.begin(), a.end())) {
            cout << "Invalid input! (the alphabet might be wrong)\n";
            exit(1);
        }
    }

    void Solve(){
        addConcat();
        setValue();
        checkIfValidInput();
        toPolish();
        // getPolish();
        toSyntaxTree();
        normalizeTree();
        // getTree(root);
        computeENFA(root, ++states);
        getEdges();
        getStartFinishStates();
    }

    // generates the .dot file needed for visualizing the graph
    void printToDotFile() {
        ofstream dotFout("main.dot");
        dotFout << "digraph ENFA {\n";
        dotFout << "\trankdir = LR;\n";
        dotFout << "\tnode [shape = circle]; " << root.startState << ";\n";
        dotFout << "\tnode [shape = doublecircle]; " << root.finishState << ";\n";
        dotFout << "\tnode [shape = plaintext];\n";
        dotFout << "\t\"\" -> " << root.startState << " [label = \"start\"];\n";
        dotFout << "\tnode [shape = circle];\n";
        for (auto it : edges) {
            dotFout << "\t" << it.source << " -> " << it.dest << " [ label = \"" << (it.value == '$' ? "λ" : string(1, it.value)) << "\" ];\n";
        }

        dotFout << "}";
    }
};

int main(){
    bool isPrefixForm = false;
    string alphabet, expr;

    cout << "Alphabet: "; cin >> alphabet;
    cout << "Expression: "; cin >> expr;
    cout << "Prefix (0) or Infix (1) form? Input: (0 / 1) -> "; cin >> isPrefixForm; 

    ExprToENFA A(alphabet, expr, !isPrefixForm);
    A.Solve();
    A.printToDotFile();
    return 0;
}