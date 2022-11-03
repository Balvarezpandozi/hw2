#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

// DEBUGGING
const bool DEBUG = false;

// TOKENIZER TYPE CONSTANTS
const string NEW_LINE = "NEW_LINE";
const string INDENT = "INDENT";
const string EQUALS = "EQUAL";
const string COLON = "COLON";
const string SEMICOLON = "SEMICOLON";
const string COMMA = "COMMA";
const string SPACE = "SPACE";
const string PLUS = "PLUS";
const string MINUS = "MINUS";
const string MULTIPLY = "MULTIPLY";
const string DIVIDE = "DIVIDE";
const string GREATER_THAN = "GREATER_THAN";
const string LESS_THAN = "LESS_THAN";
const string OPEN_PARENTHESES = "OPEN_PARENTHESES";
const string CLOSE_PARENTHESES = "CLOSE_PARENTHESES";
const string OPEN_BRACKET = "OPEN_BRACKET";
const string CLOSE_BRACKET = "CLOSE_BRACKET";
const string OPEN_BRACE = "OPEN_BRACE";
const string CLOSE_BRACE = "CLOSE_BRACE";
const string NUMBER = "NUMBER";
const string IDENTIFIER = "IDENTIFIER";
const string STRING = "STRING";
const string PRINT = "PRINT";
const string COMMENT = "COMMENT";
const string IF = "IF";
const string ELSE = "ELSE";
const string IS_EQUAL = "IS_EQUAL";

//Each Token represents a word, a symbol or a number
struct token {
    string type;
    string value = "";
    int startPosition;
};

//AST Node Types
const string AST_PROGRAM = "PROGRAM";
const string AST_ASSIGMENT = "ASSIGMENT";
const string AST_OPERATION = "OPERATION";
const string AST_NUMBER = "NUMBER";
const string AST_STRING = "STRING";
const string AST_LIST = "LIST";
const string AST_LIST_INDEX = "LIST_INDEX";
const string AST_IDENTIFIER = "IDENTIFIER";
const string AST_FUNCTION_CALL = "FUNCTION_CALL";
const string AST_EXPRESSION = "EXPRESSION";
const string AST_BODY = "BODY";

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
        vector<int> list;
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

    if (DEBUG){
        //PRINT ALL VARIABLES
        for (auto const& x : variables) {
            cout << x.first << " : ";
            if (x.second.list.size() > 0) {
                cout << "[ ";
                for (int i = 0; i < x.second.list.size(); i++) {
                    cout << x.second.list[i] << " ";
                }
                cout << "]" << endl;
            } else {
                cout << x.second.integer << endl;
            }
        }
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

        if (lookahead == " ") {
            for (int i = 0; i < 3; i++) {
                currIndex++;
                lookahead = input[currIndex];
                if (lookahead != " ") {
                    break;
                }
                if (i == 2) {
                    tokens.push_back(makeOneSymbolToken(INDENT, "    ", tokenStartPos));
                    currIndex ++;
                }
                
            }
            continue;
        }

        if (lookahead == ";") {
            currIndex ++;
            continue;
        }

        if (lookahead == "\n") {
            tokens.push_back(makeOneSymbolToken(NEW_LINE, lookahead, tokenStartPos));
            currIndex ++;
            continue;
        }

        if (lookahead == "=") {
            currIndex ++;
            lookahead = input[currIndex];
            if (lookahead == "=") {
                tokens.push_back(makeOneSymbolToken(IS_EQUAL, "==", tokenStartPos));
                currIndex ++;
                continue;
            } else {
                tokens.push_back(makeOneSymbolToken(EQUALS, "=", tokenStartPos));
                continue;
            }
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

        if (lookahead == "\"") {
            string str = "";
            currIndex ++;
            lookahead = input[currIndex];
            while (lookahead != "\"") {
                str += lookahead;
                currIndex ++;
                lookahead = input[currIndex];
            }
            token t;
            t.type = STRING;
            t.value = str;
            t.startPosition = tokenStartPos;
            tokens.push_back(t);
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
            t.startPosition = tokenStartPos;
            tokens.push_back(t);
            continue;
        }

        if (isalpha(lookahead[0])) {
            string identifier = "";
            while ((isalpha(input[currIndex]) || isdigit(input[currIndex]) || input[currIndex] == '_') && currIndex < input.length()) {
                identifier += input[currIndex];
                currIndex ++;
            }
            if (identifier == "print") {
                tokens.push_back(makeOneSymbolToken(PRINT, identifier, tokenStartPos));
            } else if (identifier == "if") {
                tokens.push_back(makeOneSymbolToken(IF, identifier, tokenStartPos));
            } else if (identifier == "else") {
                tokens.push_back(makeOneSymbolToken(ELSE, identifier, tokenStartPos));
            } else {
                tokens.push_back(makeOneSymbolToken(IDENTIFIER, identifier, tokenStartPos));
            }
            continue;
        }

        cout << "Error: Invalid character -> " << lookahead << " <- " << endl;
        currIndex ++;
    }
    
    if (DEBUG){
        //Print tokens
        cout << "TOKENS: " << tokens.size() << " tokens found" << endl;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i].type == NEW_LINE){
                cout << tokens[i].type << " " << "\\n" << " " << tokens[i].startPosition << endl;
            } else {
                cout << tokens[i].type << " " << tokens[i].value << " " << tokens[i].startPosition << endl;
            }
        }
    }

    return tokens;
}

token makeOneSymbolToken(string type, string value, int startPosition) {
    //Helper function to create a token for a single symbol
    token t;
    t.type = type;
    t.startPosition = startPosition;
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

    if (DEBUG) {
        printASTTree(root, 0);
    }
    return root; 
}

ASTNode parseExpression(vector<token> tokens, int &currToken, token &lookahead) {
    if(DEBUG){
        cout << "Analyzing token: " << lookahead.type << " at index " << currToken << endl;
    }
    
    if (lookahead.type == NEW_LINE) {
        if(DEBUG){
            cout<<"END OF LINE"<<endl;
        }
        ASTNode emptyNode = ASTNode();
        emptyNode.type = "LINE_END";
        return emptyNode;
    }
    
    if (lookahead.type == IDENTIFIER) {
        if(DEBUG){
            cout<<"IDENTIFIER"<<endl;
        }
        ASTNode assigment = ASTNode();
        assigment.symbol = lookahead.value;
        currToken ++;
        lookahead = tokens[currToken];
        if(lookahead.type == EQUALS) {
            if(DEBUG){
                cout << "IDENTIFIER then EQUALS" << endl;
            }
            assigment.type = AST_ASSIGMENT;
            currToken ++;
            lookahead = tokens[currToken];
            ASTNode expression = parseExpression(tokens, currToken, lookahead);
            assigment.expression.push_back(expression);
            return assigment;
        } else if (lookahead.type == IS_EQUAL) {
            if(DEBUG){
                cout << "IDENTIFIER then IS_EQUAL" << endl;
            }
            assigment.type = IDENTIFIER;
            ASTNode operation = ASTNode();
            operation.type = AST_OPERATION;
            operation.value = "==";
            operation.parameters.push_back(assigment);
            currToken ++;
            lookahead = tokens[currToken];
            operation.parameters.push_back(parseExpression(tokens, currToken, lookahead));
            return operation;
        } else if (lookahead.type == NEW_LINE) {
            if(DEBUG){
                cout << "IDENTIFIER then NEW_LINE" << endl;
            }
            assigment.type = AST_IDENTIFIER;
            return assigment;
        } else if (lookahead.type == PLUS) {
            if(DEBUG){
                cout << "IDENTIFIER then PLUS" << endl;
            }
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
            if(DEBUG){
                cout << "IDENTIFIER then CLOSE_PARENTHESES" << endl;
            }
            assigment.type = AST_IDENTIFIER;
            return assigment;
        } else if (lookahead.type == OPEN_BRACKET) {
            if(DEBUG){
                cout << "IDENTIFIER then OPEN_BRACKET" << endl;
            }
            ASTNode listIndex = ASTNode();
            listIndex.type = AST_LIST_INDEX;
            listIndex.symbol = assigment.symbol;
            while (lookahead.type != CLOSE_BRACKET) {
                currToken ++;
                lookahead = tokens[currToken];
                listIndex.parameters.push_back(parseExpression(tokens, currToken, lookahead));
            }
            currToken ++;
            lookahead = tokens[currToken];
            if(lookahead.type == EQUALS) {
                if(DEBUG){
                    cout << "IDENTIFIER then OPEN_BRACKET then EQUALS" << endl;
                }
                currToken ++;
                lookahead = tokens[currToken];
                assigment.type = AST_ASSIGMENT;
                listIndex.expression.push_back(parseExpression(tokens, currToken, lookahead));
                assigment.expression.push_back(listIndex);
                return assigment;
            }
            return listIndex;
        }
    }
    
    if (lookahead.type == NUMBER) {
        if(DEBUG){
            cout<<"NUMBER"<<endl;
        }
        ASTNode number = ASTNode();
        number.type = AST_NUMBER;
        number.value = lookahead.value;
        currToken ++;
        lookahead = tokens[currToken];
        if(lookahead.type == PLUS) {
            if(DEBUG){
                cout << "NUMBER then PLUS" << endl;
            }
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

    if (lookahead.type == STRING) {
        if(DEBUG){
            cout<<"STRING"<<endl;
        }
        ASTNode string = ASTNode();
        string.type = AST_STRING;
        string.value = lookahead.value;
        currToken ++;
        lookahead = tokens[currToken];
        return string;
    }

    if (lookahead.type == IF) {
        if(DEBUG){
            cout<<"IF"<<endl;
        }
        ASTNode ifStatement = ASTNode();
        ifStatement.type = AST_FUNCTION_CALL;
        ifStatement.symbol = lookahead.value;
        currToken ++;
        lookahead = tokens[currToken];
        ASTNode condition = parseExpression(tokens, currToken, lookahead);
        ifStatement.parameters.push_back(condition);
        if (lookahead.type == COLON) {
            currToken ++;
            lookahead = tokens[currToken];
        } else {
            currToken ++;
            lookahead = tokens[currToken];
            if (lookahead.type == COLON) {
                currToken ++;
                lookahead = tokens[currToken];
            } else {
                cout << "ERROR: Expected colon after if statement" << endl;
                exit(1);
            }
        }
        ASTNode body = ASTNode();
        body.type = AST_BODY;
        body.symbol = "if";
        while (currToken < tokens.size() && lookahead.type != ELSE) {
            if (lookahead.type == NEW_LINE) {
                if (currToken + 1 < tokens.size()) {
                    currToken ++;
                    lookahead = tokens[currToken];
                    if (lookahead.type == INDENT) {
                        currToken ++;
                        lookahead = tokens[currToken];
                        body.children.push_back(parseExpression(tokens, currToken, lookahead));
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            } else {
                currToken ++;
                lookahead = tokens[currToken];
            }
        }
        ASTNode elseBody = ASTNode();
        if (lookahead.type != ELSE) {
            currToken --;
            lookahead = tokens[currToken];
            ifStatement.expression.push_back(body);
            return ifStatement;
        } else {
            elseBody.type = AST_BODY;
            elseBody.symbol = lookahead.value;
            currToken ++;
            lookahead = tokens[currToken];
            if (lookahead.type == COLON) {
                currToken ++;
                lookahead = tokens[currToken];
            } else {
                cout << "ERROR: Expected colon after else statement" << endl;
                exit(1);
            }
        }
        while (currToken < tokens.size()) {
            if (lookahead.type == NEW_LINE) {
                if (currToken + 1 < tokens.size()) {
                    currToken ++;
                    lookahead = tokens[currToken];
                    if (lookahead.type == INDENT) {
                        currToken ++;
                        lookahead = tokens[currToken];
                        elseBody.children.push_back(parseExpression(tokens, currToken, lookahead));
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            } else {
                currToken ++;
                lookahead = tokens[currToken];
            }
        }
        currToken --;
        lookahead = tokens[currToken];
        ifStatement.expression.push_back(body);
        ifStatement.expression.push_back(elseBody);
        return ifStatement;
    }

    if (lookahead.type == PRINT) {
        if(DEBUG){
            cout<<"PRINT"<<endl;
        }
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
        if(DEBUG){
            cout<<"OPEN_PARENTHESES"<<endl;
        }
        ASTNode expression = ASTNode();
        expression.type = AST_EXPRESSION;
        while (lookahead.type != CLOSE_PARENTHESES) {        
            currToken ++;
            lookahead = tokens[currToken];
            expression.expression.push_back(parseExpression(tokens, currToken, lookahead));
        }
        return expression;
    }

    if (lookahead.type == OPEN_BRACKET) {
        if(DEBUG){
            cout<<"OPEN_BRACKET"<<endl;
        }
        ASTNode list = ASTNode();
        list.type = AST_LIST;
        while (lookahead.type != CLOSE_BRACKET) {
            currToken ++;
            lookahead = tokens[currToken];
            if (lookahead.type == COMMA) {
                continue;
            }
            list.expression.push_back(parseExpression(tokens, currToken, lookahead));
        }
        currToken ++;
        lookahead = tokens[currToken];
        return list;
    }

    cout << "ERROR: TOKEN handler not implemented. Node Type: " << lookahead.type << " at character number " << lookahead.startPosition << endl;
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
    }
    if(node.expression.size() > 0) {
        cout << tabulations << "Expression " << endl;
        for (int i = 0; i < node.expression.size(); i++) {
            printASTTree(node.expression[i], tabs + 1);
        }
    } 
    if (node.parameters.size() > 0) {
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
    if (DEBUG) {
        cout << "Traversing node of type " << node.type << endl;
    }
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

    if (node.type == AST_STRING) {
        Symbol symbol = Symbol();
        symbol.type = "STRING";
        symbol.string = node.value;
        return symbol;
    }

    if (node.type == AST_OPERATION) {
        if(node.value == "+") {
            Symbol part1 = traverse(node.parameters[0]);
            Symbol part2 = traverse(node.parameters[1]);
            if (part1.type == AST_LIST && part2.type == AST_LIST) {
                Symbol symbol = Symbol();
                symbol.type = AST_LIST;
                vector<int> newList;
                for (int i = 0; i < part1.list.size(); i++) {
                    newList.push_back(part1.list[i]);
                }
                for (int i = 0; i < part2.list.size(); i++) {
                    newList.push_back(part2.list[i]);
                }
                symbol.list = newList;
                return symbol;
            }
            
            Symbol result = Symbol();
            result.type = "NUMBER";
            result.integer = part1.integer + part2.integer;
            return result;
        }

        if(node.value == "==") {
            Symbol part1 = traverse(node.parameters[0]);
            Symbol part2 = traverse(node.parameters[1]);
            Symbol result = Symbol();
            result.type = "BOOLEAN";
            result.boolean = part1.integer == part2.integer;
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
            Symbol symbol;
            for (int i = 0; i < node.expression[0].expression.size(); i++){
                Symbol toPrint = traverse(node.expression[0].expression[i]); 
                if (toPrint.type == AST_NUMBER) {
                    cout << toPrint.integer << endl;
                }
                if (toPrint.type == AST_LIST) {
                    cout << "[";
                    for (int i = 0; i < toPrint.list.size(); i++) {
                        cout << toPrint.list[i];
                        if (i != toPrint.list.size() - 1) {
                            cout << ", ";
                        }
                    }
                    cout << "]" << endl;
                }
                if (toPrint.type == AST_STRING) {
                    cout << toPrint.string << " ";
                }
            }
            cout << endl;
            return symbol;
        }

        if (node.symbol == "if") {
            if (node.expression.size() == 0) {
                return Symbol();
            }
            Symbol condition = traverse(node.parameters[0]);
            if (condition.boolean) {
                for (int i = 0; i < node.expression[0].children.size(); i++) {
                    traverse(node.expression[0].children[i]);
                }
            } else {
                if(node.expression.size() > 1) {
                    for (int i = 0; i < node.expression[1].children.size(); i++) {
                        traverse(node.expression[1].children[i]);
                    }
                }
            }
        }

    }

    if (node.type == AST_EXPRESSION) {
        return traverse(node.expression[0]);
    }

    if (node.type == AST_LIST) {
        Symbol symbol = Symbol();
        symbol.type = "LIST";
        vector<int> list;
        for (int i = 0; i < node.expression.size(); i++) {
            list.push_back(traverse(node.expression[i]).integer);
        }
        symbol.list = list;
        return symbol;
    }

    if (node.type == AST_LIST_INDEX) {
        if(variables.find(node.symbol) != variables.end()) {
            Symbol symbol = variables[node.symbol];
            if (node.expression.size() > 0) {
                Symbol expression = traverse(node.expression[0]);
                Symbol parameter = traverse(node.parameters[0]);
                symbol.list[parameter.integer] = expression.integer;
                return symbol;
            } else {
                Symbol result = Symbol();
                result.type = "NUMBER";
                result.integer = symbol.list[traverse(node.parameters[0]).integer];
                return result;
            }
        } else {
            cout << "ERROR: Variable " << node.symbol << " not found" << endl;
            exit(1);
        }
    }

    return Symbol();
}