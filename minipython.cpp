#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

// TOKENIZER TYPE CONSTANTS
const string NEW_LINE = "NEW LINE";
const string EQUALS = "EQUAL";
const string COLON = "COLON";
const string SEMICOLON = "SEMICOLON";
const string COMMA = "COMMA";
const string SPACE = "SPACE";
const string PLUS = "PLUS";
const string MINUS = "MINUS";
const string MULTIPLY = "MULTIPLY";
const string DIVIDE = "DIVIDE";
const string GREATER_THAN = "GREATER THAN";
const string LESS_THAN = "LESS THAN";
const string OPEN_PARENTHESES = "OPEN PARENTHESES";
const string CLOSE_PARENTHESES = "CLOSE PARENTHESES";
const string OPEN_BRACKET = "OPEN BRACKET";
const string CLOSE_BRACKET = "CLOSE BRACKET";
const string OPEN_BRACE = "OPEN BRACE";
const string CLOSE_BRACE = "CLOSE BRACE";
const string NUMBER = "NUMBER";
const string IDENTIFIER = "IDENTIFIER";
const string PRINT = "PRINT";
const string COMMENT = "COMMENT";

//Each Token represents a word, a symbol or a number
struct token {
    string type;
    string value = "";
    int startPostition;
};

//AST Node Types
const string AST_PROGRAM = "PROGRAM";
const string AST_ASSIGMENT = "ASSIGMENT";
const string AST_OPERATION = "OPERATION";
const string AST_NUMBER = "NUMBER";
const string AST_IDENTIFIER = "IDENTIFIER";
const string AST_FUNCTION_CALL = "FUNCTION_CALL";
const string AST_EXPRESSION = "EXPRESSION";

class ASTNode {
    public:
        string type;
        string value;
        string symbol;
        vector<ASTNode> children;
        vector<ASTNode> parameters;
        vector<ASTNode> expression;
};

//Interpreter Variables functions and types
class Symbol{
    public:
        string type;
        int integer;
        string string;
        bool boolean;
};

unordered_map<string, Symbol> variables;

//Function Declarations
string getInput(string input);
vector<token> lexer(string input);
token makeOneSymbolToken(string type, string value, int startPosition);
ASTNode parseTree(vector<token> tokens);
ASTNode parseExpression(vector<token> tokens, int &currToken, token &lookahead);
void printASTTree(ASTNode node, int tabs);
void interpret(ASTNode ASTRoot);
Symbol traverse(ASTNode node);

int main(int argc, char* argv[]) {
    //Obtain input from file string
    string pythonFile = getInput(argv[1]);
    
    //Lexer
    vector<token> tokens = lexer(pythonFile);

    //Parser
    ASTNode ast = parseTree(tokens);

    //Interpreter
    interpret(ast);

    for (auto const& x : variables) {
        cout << x.first << ": " << x.second.integer << endl;
    }

    return 0;
}

//INPUT FUNCTION
string getInput(string input) {
    //Obtains a string from the input python file
    ifstream inputFile(input);
    if (!inputFile.is_open()) {
        cout << "No parameters were passed" << endl;
        exit(1);
    }
    return string((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
}

//LEXER (TOKENIZER)
vector<token> lexer(string input) {
    //Generates tokens for every symbol, identifier, keyword or number
    vector<token> tokens;
    int currIndex = 0;

    while (currIndex < input.length()) {
        int tokenStartPos = currIndex;
        string lookahead = "";
        lookahead += input[currIndex];

        if (lookahead == " " || lookahead == ";") {
            currIndex ++;
            continue;
        }

        if (lookahead == "\n") {
            tokens.push_back(makeOneSymbolToken(NEW_LINE, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "=") {
            tokens.push_back(makeOneSymbolToken(EQUALS, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == ":") {
            tokens.push_back(makeOneSymbolToken(COLON, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == ",") {
            tokens.push_back(makeOneSymbolToken(COMMA, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "+") {
            tokens.push_back(makeOneSymbolToken(PLUS, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "-") {
            tokens.push_back(makeOneSymbolToken(MINUS, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "*") {
            tokens.push_back(makeOneSymbolToken(MULTIPLY, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "/") {
            tokens.push_back(makeOneSymbolToken(DIVIDE, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == ">") {
            tokens.push_back(makeOneSymbolToken(GREATER_THAN, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "<") {
            tokens.push_back(makeOneSymbolToken(LESS_THAN, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "(") {
            tokens.push_back(makeOneSymbolToken(OPEN_PARENTHESES, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == ")") {
            tokens.push_back(makeOneSymbolToken(CLOSE_PARENTHESES, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "[") {
            tokens.push_back(makeOneSymbolToken(OPEN_BRACKET, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "]") {
            tokens.push_back(makeOneSymbolToken(CLOSE_BRACKET, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "{") {
            tokens.push_back(makeOneSymbolToken(OPEN_BRACE, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "}") {
            tokens.push_back(makeOneSymbolToken(CLOSE_BRACE, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if(lookahead == "#") {
            currIndex ++;
            lookahead = input[currIndex];
            while (lookahead != "\n") {
                currIndex ++;
                lookahead = input[currIndex];
            }
            continue;
        }

        if (isdigit(lookahead[0])) {
            string number = "";
            while (isdigit(input[currIndex]) && currIndex < input.length()) {
                number += input[currIndex];
                currIndex ++;
            }
            token t;
            t.type = NUMBER;
            t.value = number;
            t.startPostition = tokenStartPos;
            tokens.push_back(t);
            continue;
        }

        if (isalpha(lookahead[0])) {
            string identifier = "";
            while (isalpha(input[currIndex]) && currIndex < input.length()) {
                identifier += input[currIndex];
                currIndex ++;
            }
            if (identifier == "print") {
                tokens.push_back(makeOneSymbolToken(PRINT, identifier, tokenStartPos));
            } else {
                tokens.push_back(makeOneSymbolToken(IDENTIFIER, identifier, tokenStartPos));
            }
            continue;
        }

        cout << "Error: Invalid character -> " << lookahead << " <- " << endl;
        currIndex ++;
    }
    
    //Print tokens
    cout << "TOKENS: " << tokens.size() << " tokens found" << endl;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == NEW_LINE){
            cout << tokens[i].type << " " << "\\n" << " " << tokens[i].startPostition << endl;
        } else {
            cout << tokens[i].type << " " << tokens[i].value << " " << tokens[i].startPostition << endl;
        }
    }

    return tokens;
}

token makeOneSymbolToken(string type, string value, int startPostition) {
    //Helper function to create a token for a single symbol
    token t;
    t.type = type;
    t.startPostition = startPostition;
    t.value = value;
    return t;
}

//TREE PARSING
ASTNode parseTree(vector<token> tokens) {
    ASTNode root = ASTNode();
    root.type = "PROGRAM";

    int currToken = 0;
    token lookahead = tokens[currToken];

    while (currToken < tokens.size()) {
        cout << "While loop: currToken: " << currToken << " Size: " << tokens.size() << endl;
        if (lookahead.type == NEW_LINE) {
            if (currToken + 1 < tokens.size()) {
                currToken ++;
                lookahead = tokens[currToken];
            } else {
                break;
            }
        } else {
            ASTNode statement = parseExpression(tokens, currToken, lookahead);
            root.children.push_back(statement);
            if (currToken + 1 < tokens.size()) {
                currToken ++;
                lookahead = tokens[currToken];
            } else {
                break;
            }
        }
    }

    printASTTree(root, 0);

    return root; 
}

ASTNode parseExpression(vector<token> tokens, int &currToken, token &lookahead) {
    cout << "Analyzing token: " << lookahead.type << " at index " << currToken << endl;
    if (lookahead.type == NEW_LINE) {
        cout<<"END OF LINE"<<endl;
        ASTNode emptyNode = ASTNode();
        emptyNode.type = "LINE_END";
        return emptyNode;
    }
    
    if (lookahead.type == IDENTIFIER) {
        cout<<"IDENTIFIER"<<endl;
        ASTNode assigment = ASTNode();
        assigment.symbol = lookahead.value;
        currToken ++;
        lookahead = tokens[currToken];
        if(lookahead.type == EQUALS) {
            cout<<"IDENTIFIER then EQUALS"<<endl;
            assigment.type = AST_ASSIGMENT;
            currToken ++;
            lookahead = tokens[currToken];
            ASTNode expression = parseExpression(tokens, currToken, lookahead);
            assigment.expression.push_back(expression);
            return assigment;
        } else if (lookahead.type == NEW_LINE) {
            cout<<"IDENTIFIER then NEW_LINE"<<endl;
            assigment.type = AST_IDENTIFIER;
            return assigment;
        } else if (lookahead.type == PLUS) {
            cout<<"IDENTIFIER then PLUS"<<endl;
            currToken++;
            lookahead = tokens[currToken];
            assigment.type = IDENTIFIER;
            ASTNode expression = ASTNode();
            expression.type = AST_OPERATION;
            expression.value = "+";
            expression.parameters.push_back(assigment);
            expression.parameters.push_back(parseExpression(tokens, currToken, lookahead));
            return expression;
        } else if (lookahead.type == CLOSE_PARENTHESES) {
            cout << "IDENTIFIER then CLOSE_PARENTHESES" << endl;
            assigment.type = AST_IDENTIFIER;
            return assigment;
        }
    }
    
    if (lookahead.type == NUMBER) {
        cout<<"NUMBER"<<endl;
        ASTNode number = ASTNode();
        number.type = AST_NUMBER;
        number.value = lookahead.value;
        currToken ++;
        lookahead = tokens[currToken];
        if(lookahead.type == PLUS) {
            cout << "NUMBER then PLUS" << endl;
            currToken ++;
            lookahead = tokens[currToken];
            ASTNode operation = ASTNode();
            operation.type = AST_OPERATION;
            operation.value = "+";
            operation.parameters.push_back(number);
            operation.parameters.push_back(parseExpression(tokens, currToken, lookahead));
            return operation;
        }

        return number;
    }

    if (lookahead.type == PRINT) {
        cout<<"PRINT"<<endl;
        ASTNode print = ASTNode();
        print.type = AST_FUNCTION_CALL;
        print.symbol = lookahead.value;
        currToken ++;
        lookahead = tokens[currToken];
        ASTNode expression = parseExpression(tokens, currToken, lookahead);
        print.expression.push_back(expression);
        return print;
    }

    if (lookahead.type == OPEN_PARENTHESES) {
        cout<<"OPEN_PAREN"<<endl;
        ASTNode expression = ASTNode();
        while (lookahead.type != CLOSE_PARENTHESES) {        
            currToken ++;
            lookahead = tokens[currToken];
            expression.type = AST_EXPRESSION;
            expression.expression.push_back(parseExpression(tokens, currToken, lookahead));
        }
        return expression;
    }

    return ASTNode();
}

void printASTTree(ASTNode node, int tabs) {
    string tabulations = "";
    for (int i = 0; i < tabs; i++) {
        tabulations += "\t";
    }
    cout << tabulations <<"Type: " << node.type << "-> Symbol: " << node.symbol << ". -> Value: " << node.value << endl;
    if (node.children.size() > 0) {
        cout << tabulations << "Child " << endl;
        for (int i = 0; i < node.children.size(); i++) {
            printASTTree(node.children[i], tabs + 1);
        }
    } else if(node.expression.size() > 0) {
        cout << tabulations << "Expression " << endl;
        for (int i = 0; i < node.expression.size(); i++) {
            printASTTree(node.expression[i], tabs + 1);
        }
    } else if (node.parameters.size() > 0) {
        cout << tabulations << "Parameter " << endl;
        for (int i = 0; i < node.parameters.size(); i++) {
            printASTTree(node.parameters[i], tabs + 1);
        }
    }
}

//INTERPRETER
void interpret(ASTNode node) {
    for (int i = 0; i < node.children.size(); i++) {
        traverse(node.children[i]);
    }
}

Symbol traverse(ASTNode node) {
    cout << "Traversing node: " << node.type << endl;
    if (node.type == AST_ASSIGMENT) {
        Symbol symbol = Symbol();
        string symbolName = node.symbol;
        if (node.expression.size() == 0) {
            cout << "ERROR: Assigment without expression" << endl;
            exit(1);
        }
        symbol = traverse(node.expression[0]);
        variables[symbolName] = symbol;
    }
    
    if (node.type == AST_NUMBER) {
        Symbol symbol = Symbol();
        symbol.type = "NUMBER";
        symbol.integer = stoi(node.value);
        return symbol;
    }

    if (node.type == AST_OPERATION) {
        if(node.value == "+") {
            Symbol par1 = traverse(node.parameters[0]);
            Symbol par2 = traverse(node.parameters[1]);
            Symbol result = Symbol();
            result.type = "NUMBER";
            result.integer = par1.integer + par2.integer;
            return result;
        }
    }

    if (node.type == AST_IDENTIFIER) {
        if(variables.find(node.symbol) != variables.end()) {
            return variables[node.symbol];
        } else {
            cout << "ERROR: Variable " << node.symbol << " not found" << endl;
            exit(1);
        }
    }

    if (node.type == AST_FUNCTION_CALL) {
        if (node.symbol == "print") {
            if (node.expression.size() == 0) {
                cout << "ERROR: Print without expression" << endl;
                exit(1);
            }
            Symbol symbol = traverse(node.expression[0]);
            if(symbol.type == "NUMBER") {
                cout << symbol.integer << endl;
            }
            return symbol;
        }
    }

    if (node.type == AST_EXPRESSION) {
        return traverse(node.expression[0]);
    }

    return Symbol();
}

//OUTPUT FUNCTION