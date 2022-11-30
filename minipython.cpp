#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>

using namespace std;

// DEBUGGING: used in developing to print out AST nicely as well as tokens and what nodes were being traversed
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

const string OPERATOR = "OPERATOR";

const string OPEN_PARENTHESES = "OPEN_PARENTHESES";
const string CLOSE_PARENTHESES = "CLOSE_PARENTHESES";
const string OPEN_BRACKET = "OPEN_BRACKET";
const string CLOSE_BRACKET = "CLOSE_BRACKET";
const string OPEN_BRACE = "OPEN_BRACE";
const string CLOSE_BRACE = "CLOSE_BRACE";

const string NUMBER = "NUMBER";
const string TRUE = "TRUE";
const string FALSE = "FALSE";
const string NULL_IDENTIFIER = "NULL";
const string IDENTIFIER = "IDENTIFIER";
const string STRING = "STRING";

const string PRINT = "PRINT";
const string IF = "IF";
const string ELSE = "ELSE";
const string LENGTH = "LENGTH";

const string FUNCTION_DEFINITION = "FUNCTION_DEFINITION";
const string RETURN = "RETURN";

const string BLOCK_START = "BLOCK_START";
const string BLOCK_END = "BLOCK_END";

const string COMMENT = "COMMENT";

//Each Token represents a word, a symbol or a number
struct token {
    string type;
    string value = "";
    int line;
    int position;
};

//AST Node Types
const string AST_PROGRAM = "PROGRAM";
const string AST_ASSIGMENT = "ASSIGMENT";
const string AST_OPERATION = "OPERATION";
const string AST_NUMBER = "NUMBER";
const string AST_STRING = "STRING";
const string AST_BOOLEAN = "BOOLEAN";
const string AST_NULL = "NULL";
const string AST_LIST = "LIST";
const string AST_LIST_INDEX = "LIST_INDEX";
const string AST_IDENTIFIER = "IDENTIFIER";
const string AST_FUNCTION_CALL = "FUNCTION_CALL";
const string AST_EXPRESSION = "EXPRESSION";
const string AST_BODY = "BODY";
const string AST_FUNCTION = "FUNCTION";
const string AST_PARAMETER = "PARAMETER";
const string AST_RETURN = "RETURN";
const string AST_ERROR = "ERROR";
const string AST_LENGTH = "LENGTH";

int linesToSkip = 0; //Used with function definitions, if and else statements so that those lines are nor parsed several times

//Each AST node can have children like the program root node or functions. Paarameters like conditions on if statements and arguments in functions. And Expressions which involves assigments. operations, function calls and so on.
class ASTNode {
    public:
        string type;
        string value;
        string symbol;
        vector<ASTNode> children;
        vector<ASTNode> parameters;
        vector<ASTNode> expression;
        int line;
};

//Interpreter Variables functions and types. Used for evaluation of the AST
class Symbol{
    public:
        string type;
        int integer;
        string str;
        bool boolean;
        vector<int> list;
        ASTNode function;
        int line;
};

unordered_map<string, Symbol> variables; //Stores all variables from the outer scope

//Function Declarations
string getInput(string input);
vector<vector<token>> lexer(string input);
token makeOneSymbolToken(string type, string value, int line, int position);
ASTNode parseTree(vector<vector<token>> tokens);
ASTNode parseStatement(vector<vector<token>> tokens, int line, int tokenIndex, token lookahead);
void printASTTree(ASTNode node, int tabs); //DEBUGGING
void interpret(ASTNode ASTRoot);
Symbol traverse(ASTNode node);

int main(int argc, char* argv[]) {
    //Obtain input from file string
    string pythonFile = getInput(argv[1]);

    //Lexer
    vector<vector<token>> tokens = lexer(pythonFile);

    //Parser
    ASTNode ast = parseTree(tokens);

    if (DEBUG) {
        cout << "\n" << "****************AST TREE****************" << endl;
        //Print AST Tree
        printASTTree(ast, 0);
    }

    //Interpreter
    interpret(ast);
    
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
vector<vector<token>> lexer(string input) {
    if (DEBUG) cout << "****************LEXER****************" << endl;
    vector<vector<token>> tokens; //2D vector of tokens where each vector represents a line
    vector<token> lineTokens; //Vector of tokens for each line
    int line = 1; //Stores line of code in input file
    int position = 1; //Stores character position in line
    int currIndex = 0; //Stores current index of input string (Entire file)
    int indentationLevel = 0; //Stores indentation level of current line depending on if statements and function definitions
    int indentationLength = 0; //Stores length of indentation of current line (number of spaces used for indentation)
    bool isIndent = false; //Boolean to help check for indentation when switching to new lines
    int lastIndentationLength = 0; //Stores length of indentation of previous line (number of spaces used for indentation)

    while (currIndex < input.length()) {
        string lookahead = ""; //Stores lookahead character
        lookahead += input[currIndex]; //Adds current character to lookahead

        //NEW LINE
        if (lookahead == "\n") {
            if(isIndent) {
                //Check for proper indentation on the next line
                    //Obtain length of indentation
                string indentation = "";

                //If indentation level is 1 and the indentation length is 0 (no spaces). Obtain indentation length since this is the first indented line
                if (indentationLevel == 1 && indentationLength == 0) {
                    int index = 1;
                    while (input[currIndex+index] == ' ') {
                        index ++;
                        indentationLength ++;
                    }
                }

                //Get indentation length of next line
                for (int i = 1; i <= indentationLength * indentationLevel; i++) {
                    if (input[currIndex+i] == ' ') {
                        indentation += input[currIndex+i];
                    } else {
                        break;
                    }
                }
                    //Check indentation length corresponds to the current indentation level
                if (indentation.length() != 0 && indentation.length() % (indentationLength*indentationLevel) == 0) {
                    currIndex += 1 + indentation.length();
                    position += 1 + indentation.length();
                    line ++;
                    if (lineTokens.size() > 0) tokens.push_back(lineTokens);
                    lineTokens.clear();
                    continue;
                }
                //If there is directly a next line ignore but increase line counter
                if (input[currIndex+1] == '\n') {
                    currIndex ++;
                    position ++;
                    line ++;
                    continue;
                }
                //if indentation is 0 and the indentation level is greater than one. This means that several levels of indentation ended so add corresponding breaking points.
                if (indentation.length() == 0 && indentationLevel > 1) {
                    for (int i = 1; i < indentationLevel; i++) {
                        vector<token> breakLine;
                        breakLine.push_back(makeOneSymbolToken(BLOCK_END, BLOCK_END, line, position));
                        tokens.push_back(breakLine);
                        indentationLevel --;
                    }
                }
                
                indentationLevel --; //Decrease indentation level
                if (indentationLevel == 0) isIndent = false; //If indentation level is 0 then there is no indentation
                lineTokens.push_back(makeOneSymbolToken(BLOCK_END, "BLOCK_END", line, position)); //Add BLOCK_END token to line
                indentationLength = indentationLength * indentationLevel; //Update indentation length
                line ++; //Increase line counter
                currIndex += 1 + indentation.length(); //Increase current index
                position += 1 + indentation.length(); //Increase position counter
                tokens.push_back(lineTokens); //Add line tokens to tokens vector
                lineTokens.clear(); //Clear line tokens vector
                continue;
            }

            //if there is no indentation proceed as normal
            currIndex ++; 
            position ++;
            line ++;
            if (lineTokens.size()>0) {
                tokens.push_back(lineTokens);
                lineTokens.clear();
            }
            continue;
        }

        //COMMENT
        if(lookahead == "#") {
            position ++;
            currIndex ++;
            lookahead = input[currIndex];
            while (lookahead != "\n" && currIndex <= input.length()) {
                position ++;
                currIndex ++;
                lookahead = input[currIndex];
            }
            continue;
        }

        //SPACE
        if (lookahead == " ") {
            position ++;
            currIndex ++;
            continue;
            
        }

        //SEMICOLON
        if (lookahead == ";") {
            //ignore space
            position ++;
            currIndex ++;
            continue;
        }

        //COMMA
        if(lookahead == ",") {
            lineTokens.push_back(makeOneSymbolToken(COMMA, lookahead, line, position));
            position ++;
            currIndex ++;
            continue;
        }

        //COLON 
        if(lookahead == ":") {
            lineTokens.push_back(makeOneSymbolToken(COLON, lookahead, line, position));
            position ++;
            currIndex ++;
            continue;
        }

        //EQUALS, and IS EQUAL operators
        if (lookahead == "=") {
            currIndex ++;
            lookahead = input[currIndex];
            if (lookahead == "=") {
                lineTokens.push_back(makeOneSymbolToken(OPERATOR, "==", line, position));
                position += 2;
                currIndex ++;
                continue;
            } else {
                lineTokens.push_back(makeOneSymbolToken(EQUALS, "=", line, position));
                position ++;
                continue;
            }
        }

        //GREATER and GREATER THAN operators
        if (lookahead == ">") {
            currIndex ++;
            lookahead = input[currIndex];
            if (lookahead == "=") {
                lineTokens.push_back(makeOneSymbolToken(OPERATOR, ">=", line, position));
                position += 2;
                currIndex ++;
                continue;
            } else {
                lineTokens.push_back(makeOneSymbolToken(OPERATOR, ">", line, position));
                position ++;
                continue;
            }
        }

        //LESS and LESS THAN operators
        if (lookahead == "<") {
            currIndex ++;
            lookahead = input[currIndex];
            if (lookahead == "=") {
                lineTokens.push_back(makeOneSymbolToken(OPERATOR, "<=", line, position));
                position += 2;
                currIndex ++;
                continue;
            } else {
                lineTokens.push_back(makeOneSymbolToken(OPERATOR, "<", line, position));
                position ++;
                continue;
            }
        }

        //NOT_EQUAL operator
        if (lookahead == "!") {
            currIndex ++;
            lookahead = input[currIndex];
            if (lookahead == "=") {
                lineTokens.push_back(makeOneSymbolToken(OPERATOR, "!=", line, position));
                position += 2;
                currIndex ++;
                continue;
            } else {
                cout << "ERROR: Invalid symbol " << lookahead << " at line " << line << " position " << position << endl;
                exit(1);
            }
        }

        //PLUS
        if (lookahead == "+") {
            lineTokens.push_back(makeOneSymbolToken(PLUS, "+", line, position));
            position ++;
            currIndex ++;
            continue;
        }

        //PARENTHESIS
        if (lookahead == "(") {
            lineTokens.push_back(makeOneSymbolToken(OPEN_PARENTHESES, "(", line, position));
            position ++;
            currIndex ++;
            continue;
        }

        if (lookahead == ")") {
            lineTokens.push_back(makeOneSymbolToken(CLOSE_PARENTHESES, ")", line, position));
            position ++;
            currIndex ++;
            continue;
        }

        //BRACKETS
        if (lookahead == "[") {
            lineTokens.push_back(makeOneSymbolToken(OPEN_BRACKET, "[", line, position));
            position ++;
            currIndex ++;
            continue;
        }

        if (lookahead == "]") {
            lineTokens.push_back(makeOneSymbolToken(CLOSE_BRACKET, "]", line, position));
            position ++;
            currIndex ++;
            continue;
        }

        //STRING
        if (lookahead == "\"") {
            string str = "";
            currIndex ++;
            lookahead = input[currIndex];
            while (lookahead != "\"") {
                str += lookahead;
                currIndex ++;
                lookahead = input[currIndex];
            }
            lineTokens.push_back(makeOneSymbolToken(STRING, str, line, position));
            position += str.length() + 2;
            currIndex ++;
            continue;
        }

        //INTEGER
        if (isdigit(lookahead[0])) {
            //obtain integer
            string number = "";
            while (isdigit(input[currIndex]) && currIndex < input.length()) {
                number += input[currIndex];
                currIndex ++;
            }

            //create integer token
            lineTokens.push_back(makeOneSymbolToken(NUMBER, number, line, position));
            position += number.length();
            continue;
        }

        //IDENTIFIER
        if (isalpha(lookahead[0])) {
            //obtain identifier
            string identifier = "";
            while ((isalpha(input[currIndex]) || isdigit(input[currIndex]) || input[currIndex] == '_') && currIndex < input.length()) {
                identifier += input[currIndex];
                currIndex ++;
            }

            //check if identifier is a keyword
            if (identifier == "print") {
                lineTokens.push_back(makeOneSymbolToken(PRINT, "print", line, position));
            } else if (identifier == "len") { 
                lineTokens.push_back(makeOneSymbolToken(LENGTH, "len", line, position));
            } else if (identifier == "if") {
                isIndent = true;
                indentationLevel ++;
                lineTokens.push_back(makeOneSymbolToken(IF, "if", line, position));
            } else if (identifier == "else") {
                isIndent = true;
                indentationLevel ++;
                lineTokens.push_back(makeOneSymbolToken(ELSE, "else", line, position));
            } else if (identifier == "def") {
                indentationLevel ++;
                isIndent = true;
                lineTokens.push_back(makeOneSymbolToken(FUNCTION_DEFINITION, "def", line, position));
            } else if (identifier == "return") {
                lineTokens.push_back(makeOneSymbolToken(RETURN, "return", line, position));
            } else if (identifier == "True") {
                lineTokens.push_back(makeOneSymbolToken(TRUE, "true", line, position));
            } else if (identifier == "False") {
                lineTokens.push_back(makeOneSymbolToken(FALSE, "false", line, position));
            } else if (identifier == "None") {
                lineTokens.push_back(makeOneSymbolToken(NULL_IDENTIFIER, "None", line, position));
            } else {
                lineTokens.push_back(makeOneSymbolToken(IDENTIFIER, identifier, line, position));
            }
            position += identifier.length();
            continue;
        }

        //ERROR
        cout << "Error: Invalid character -> " << lookahead << " <- " << "at line " << line << " position " << position << endl;
        exit(1);
    }

    //add line tokens to tokens
    if (lineTokens.size() > 0) {
        tokens.push_back(lineTokens);
    }

    //If DEBUGGING is on print out table with all tokens
    if (DEBUG){
        // tokens
        int tokensFound = 0;
        for (int i = 0; i<tokens.size(); i++) {
            tokensFound += tokens[i].size();
        }
        cout << "****************TOKENS****************" << endl;
        cout << "LINES " << tokens.size() << ". TOKENS: " << tokensFound << " tokens found." << endl;
        cout << "----------------------------------------------------------------------------" << endl;
        cout << "Type               \t\tValue              \t\tLine\t\tPosition" << endl;
        for (int i = 0; i<tokens.size(); i++) {
            for (int j = 0; j < tokens[i].size(); j++) {
                string type = tokens[i][j].type + string(19-tokens[i][j].type.length(), ' ');
                string value = tokens[i][j].value + string(19-tokens[i][j].value.length(), ' ');
                cout << type << "\t\t" << value << "\t\t" << tokens[i][j].line << "\t\t" << tokens[i][j].position << endl;
            }
        }

    }

    return tokens;
}

//Helper function to create a token
token makeOneSymbolToken(string type, string value, int line, int position) {
    //Helper function to create a token for a single symbol
    token t;
    t.type = type;
    t.value = value;
    t.line = line;
    t.position = position;
    return t;
}


//TREE PARSING
ASTNode parseTree(vector<vector<token>> tokens) {
    if (DEBUG) cout << "\n" << "****************TREE PARSING****************" << endl;

    //create root node: <program>
    ASTNode root = ASTNode();
    root.type = "PROGRAM";
    
    int line = 0; //current line
    int tokenIndex = 0; //current token index
    
    token lookahead; //current token

    //Iterate through all children in program
    while (line < tokens.size()) {
        lookahead = tokens[line][tokenIndex]; //Get current token
        ASTNode statement = parseStatement(tokens, line, tokenIndex, lookahead); //Parse token
        root.children.push_back(statement); //Add statement to root
        if(DEBUG) cout << endl; //DEBUGGING
        line = linesToSkip == 0 ? line + 1 : linesToSkip + line; //Update line, if indentation was involved in parsing linesToSkip will avoid parsing lines that were already parsed
        linesToSkip = 0; //Reset linesToSkip
        tokenIndex = 0; //Reset tokenIndex
    }

    return root; //Return AST
}

//Recursive AST parse function
ASTNode parseStatement(vector<vector<token>> tokens, int line, int tokenIndex, token lookahead) {
    //Create AST node
    ASTNode statement = ASTNode();

    //<IDENTIFIER>
    //If token is an identifier
    if (lookahead.type == IDENTIFIER) {
        if (DEBUG) cout << "<IDENTIFIER>"; //DEBUGGING

        statement.type = AST_IDENTIFIER; //Set type
        statement.symbol = lookahead.value; //Set symbol
        statement.line = lookahead.line; //Set line

        //If the line only contains an identifier or if the identifier is the last element in the line, return identifier
        if (tokens[line].size() == 1 || tokenIndex == tokens[line].size() - 1 || tokens[line][tokenIndex + 1].type == BLOCK_END) return statement;
        
        //Obtain following token to the identifier
        lookahead = tokens[line][tokenIndex+1];

        //<IDENTIFIER><EQUALS>
        //if following token is an equal sign
        if (lookahead.type == EQUALS) {
            if (DEBUG) cout << "<EQUALS>"; //DEBUGGING

            statement.type = AST_ASSIGMENT; //Set type
            statement.line = lookahead.line; //Set line
            ASTNode expression = parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]); //Parse expression after equal sign

            statement.expression.push_back(expression); //Add expression to assigment node
            return statement; 
        }

        //<IDENTIFIER><PLUS>
        //if following token is a plus sign
        if (lookahead.type == PLUS) {
            if (DEBUG) cout << "<PLUS>";

            ASTNode operation = ASTNode();
            operation.type = AST_OPERATION; //Set type
            operation.line = lookahead.line; //Set line
            operation.value = "+"; //Set value

            operation.parameters.push_back(statement); //Add identifier to parameters
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2])); //Add expression after plus sign to parameters

            return operation;
        }

        //<IDENTIFIER><OPEN_PARENTHESIS>
        //if following token is an open parentheses
        if (lookahead.type == OPEN_PARENTHESES) {
            if (DEBUG) cout << "<OPEN_PARENTHESIS>"; //DEBUGGING

            ASTNode functionCall = ASTNode();
            functionCall.type = AST_FUNCTION_CALL; //Set type
            functionCall.symbol = statement.symbol; //Set symbol
            functionCall.line = lookahead.line; //Set line
            int tokenIncrement = 2; //Increment to skip open parentheses
            lookahead = tokens[line][tokenIndex+tokenIncrement]; //Get next token
            while (lookahead.type != CLOSE_PARENTHESES) {
                //Ignore commas
                if (lookahead.type == COMMA) {
                    tokenIncrement++;
                    lookahead = tokens[line][tokenIndex+tokenIncrement];
                    continue;
                }
                ASTNode expression = parseStatement(tokens, line, tokenIndex+tokenIncrement, lookahead); //parse parameter
                functionCall.parameters.push_back(expression); //Add parameter to function call
                tokenIncrement ++; //Increment to skip parameter
                lookahead = tokens[line][tokenIndex+tokenIncrement]; //Get next token
            }
            if (DEBUG) cout << "<CLOSE_PARENTHESES>"; //DEBUGGING
            return functionCall;
        }

        //<IDENTIFIER><CLOSE_PARENTHESIS>
        //if following token is a close parentheses
        if (lookahead.type == CLOSE_PARENTHESES) {
            if (DEBUG) cout << "<CLOSE_PARENTHESIS>";
            return statement;
        }

        //<IDENTIFIER><OPEN_BRACKET>
        if (lookahead.type == OPEN_BRACKET) {
            if (DEBUG) cout << "<OPEN_BRACKET>"; //DEBUGGING
            ASTNode listIndex = ASTNode();
            listIndex.type = AST_LIST_INDEX; //Set type
            listIndex.symbol = statement.symbol; //Set symbol
            listIndex.line = lookahead.line; //Set line
            listIndex.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2])); //Add expression after open bracket to parameters
            if (DEBUG) cout << "<CLOSE_BRACKET>"; //DEBUGGING

            int tokenIncrement = 2; //Increment to skip open bracket

            while (lookahead.type != CLOSE_BRACKET) { 
                if (tokens[line].size() > tokenIndex + tokenIncrement) {
                    lookahead = tokens[line][tokenIndex+tokenIncrement];
                    tokenIncrement++;
                } else {
                    cout << "Error: Missing ] at line " << line << endl;
                    exit(1);
                }
            }

            //Check if there are more tokens after the close bracket
            if (tokens[line].size() > tokenIndex+tokenIncrement) {
                lookahead = tokens[line][tokenIndex+tokenIncrement];
            } else {
                //Not other tokens return listIndex
                return listIndex;
            }
            
            //<IDENTIFIER><OPEN_BRACKET>...<CLOSE_BRACKET><EQUALS>
            //If following token is an equal sign
            if (lookahead.type == EQUALS) {
                if (DEBUG) cout << "<EQUALS>";
                statement.type = AST_ASSIGMENT;
                statement.line = lookahead.line;
                lookahead = tokens[line][tokenIndex+tokenIncrement+1];
                listIndex.expression.push_back(parseStatement(tokens, line, tokenIndex+tokenIncrement+1, lookahead));
                statement.expression.push_back(listIndex);
                return statement;
            }

            //<IDENTIFIER><OPEN_BRACKET>...<CLOSE_BRACKET><PLUS>
            if (lookahead.type == PLUS) { 
                if (DEBUG) cout << "<OPERATOR>"; //DEBUGGING
                ASTNode operation = ASTNode();
                operation.type = AST_OPERATION;
                operation.line = lookahead.line;
                operation.value = lookahead.value;

                operation.parameters.push_back(listIndex);
                operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+tokenIncrement+1, tokens[line][tokenIndex+tokenIncrement+1]));

                return operation;
            }

            return listIndex;
        }

        //<IDENTIFIER><OPERATOR>
        if (lookahead.type == OPERATOR) {
            if (DEBUG) cout << "<OPERATOR>";

            ASTNode operation = ASTNode();
            operation.type = AST_OPERATION;
            operation.line = lookahead.line;
            operation.value = lookahead.value;

            operation.parameters.push_back(statement);
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]));

            return operation;
        }

        //<IDENTIFIER><OPEN_PARENTHESIS>
        if (lookahead.type == OPEN_PARENTHESES) {
            if (DEBUG) cout << "<OPEN_PARENTHESIS>";

            ASTNode functionCall = ASTNode();
            functionCall.type = AST_FUNCTION_CALL;
            functionCall.line = lookahead.line;
            functionCall.symbol = statement.symbol;

            //If the function call has parameters
            int tokenIncrement = 1;
            while (tokenIndex+2+tokenIncrement < tokens[line].size()) {
                lookahead = tokens[line][tokenIndex+2+tokenIncrement];
                //if end of parameters reached
                if (lookahead.type == CLOSE_PARENTHESES) break;

                functionCall.parameters.push_back(parseStatement(tokens, line, tokenIndex+2+tokenIncrement, lookahead));
            }

            return functionCall;
        }

        return statement;
    }

    //<OPEN_PARENTHESIS>
    //If token is an open parenthesis
    if (lookahead.type == OPEN_PARENTHESES) {
        if (DEBUG) cout << "<OPEN_PARENTHESIS>";

        ASTNode expression = ASTNode();
        expression.type = AST_EXPRESSION;
        expression.line = lookahead.line;

        int tokenIncrement = 1;
        while (lookahead.type != CLOSE_PARENTHESES) {
            //Get next token
            lookahead = tokens[line][tokenIndex+tokenIncrement];

            //Check if it is a comma if it is skip it
            if (lookahead.type == COMMA) {
                tokenIncrement++;
                lookahead = tokens[line][tokenIndex+tokenIncrement];
            }

            //Check if it is a close parenthesis if it is break
            if (lookahead.type == CLOSE_PARENTHESES) break;

            //Parse next token
            ASTNode statement = parseStatement(tokens, line, tokenIndex+tokenIncrement, lookahead);
            
            //Add to parameters
            expression.expression.push_back(statement);
            tokenIncrement++;

            //move to the next comma or closing parenthesis
            for (int i = tokenIndex+tokenIncrement; i < tokens[line].size(); i++) {
                if (tokens[line][i].type == COMMA || tokens[line][i].type == CLOSE_PARENTHESES) {
                    break;
                }
                tokenIncrement ++;
            }
            
        }

        return expression;
    }

    //<OPEN BRACKET>
    //If token is an open bracket
    if (lookahead.type == OPEN_BRACKET) {
        if (DEBUG) cout << "<OPEN_BRACKET>";

        ASTNode list = ASTNode();
        list.type = AST_LIST;
        list.line = lookahead.line;
        int tokenIncrement = 1;

        while (lookahead.type != CLOSE_BRACKET) {
            //Get token
            lookahead = tokens[line][tokenIndex+tokenIncrement];

            //Check if it is a comma if it is skip it
            if (lookahead.type == COMMA) {
                tokenIncrement++;
                lookahead = tokens[line][tokenIndex+tokenIncrement];
            }

            //Check if it is a close bracket if it is break
            if (lookahead.type == CLOSE_BRACKET) break;

            //Parse next token
            ASTNode element = parseStatement(tokens, line, tokenIndex+tokenIncrement, lookahead);

            //Add to list expression
            list.expression.push_back(element);

            tokenIncrement++;
        }

        if (DEBUG) cout << "<CLOSE_BRACKET>";

        tokenIncrement ++;
        if (tokenIndex + tokenIncrement < tokens[line].size()) {
            lookahead = tokens[line][tokenIndex+tokenIncrement];
        } else {
            return list;
        }

        if (lookahead.type == PLUS && tokens[line][tokenIndex+tokenIncrement+1].type != BLOCK_END) {
            if (DEBUG) cout << "<OPERATOR>";
            ASTNode operation = ASTNode();
            operation.type = AST_OPERATION;
            operation.line = lookahead.line;
            operation.value = lookahead.value;

            operation.parameters.push_back(list);
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+tokenIncrement+1, tokens[line][tokenIndex+tokenIncrement+1]));

            return operation;
        }
    }

    //<NUMBER>
    //If token is a number
    if (lookahead.type == NUMBER) {
        if (DEBUG) cout << "<NUMBER>";

        statement.type = AST_NUMBER;
        statement.line = lookahead.line;
        statement.value = lookahead.value;

        //If the line only contains a number or if the number is the last element in the line
        if (tokens[line].size() == 1 || tokenIndex == tokens[line].size() - 1) return statement;

        lookahead = tokens[line][tokenIndex+1];

        //<NUMBER><PLUS>
        if (lookahead.type == PLUS) {
            if (DEBUG) cout << "<PLUS>";

            ASTNode operation = ASTNode();
            operation.type = AST_OPERATION;
            operation.line = lookahead.line;
            operation.value = "+";

            operation.parameters.push_back(statement);
            operation.parameters.push_back(parseStatement(tokens, line, tokenIndex+2, tokens[line][tokenIndex+2]));

            return operation;
        }

        return statement;
    }

    //<STRING>
    //If token is a string
    if (lookahead.type == STRING) {
        if (DEBUG) cout << "<STRING>";

        statement.type = AST_STRING;
        statement.value = lookahead.value;
        statement.line = lookahead.line;

        return statement;
    }

    //<BOOLEAN>
    //If token is a boolean
    if (lookahead.type == TRUE || lookahead.type == FALSE) {
        if (DEBUG) cout << "<BOOLEAN>";

        statement.type = AST_BOOLEAN;
        statement.line = lookahead.line;
        statement.value = lookahead.value;

        return statement;
    }

    //<NULL>
    //If token is null
    if (lookahead.type == NULL_IDENTIFIER) {
        if (DEBUG) cout << "<NULL>";

        statement.type = AST_NULL;
        statement.line = lookahead.line;
        statement.value = lookahead.value;

        return statement;
    }

    //<PRINT>
    //If token is print
    if (lookahead.type == PRINT) {
        if (DEBUG) cout << "<PRINT>";

        ASTNode print = ASTNode();
        print.type = AST_FUNCTION_CALL;
        print.symbol = lookahead.value;
        print.line = lookahead.line;

        print.expression.push_back(parseStatement(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]));
        return print;
    }

    //<LENGTH>
    if (lookahead.type == LENGTH) {
        if (DEBUG) cout << "<LENGTH>";

        ASTNode length = ASTNode();
        length.type = AST_FUNCTION_CALL;
        length.line = lookahead.line;
        length.symbol = lookahead.value;

        length.expression.push_back(parseStatement(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]));
        return length;
    }

    //<IF>
    if (lookahead.type == IF) {
        if (DEBUG) cout << "<IF>";

        ASTNode ifStatement = ASTNode();
        ifStatement.type = AST_FUNCTION_CALL;
        ifStatement.symbol = lookahead.value;
        ifStatement.line = lookahead.line;

        ifStatement.parameters.push_back(parseStatement(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]));

        //Check for colon and new line
        int tokenIncrement = 2;
        for (int i = tokenIndex+tokenIncrement; i<tokens[line].size(); i++) {
            lookahead = tokens[line][i];
            if (lookahead.type == COLON) {
                if (DEBUG) cout << "<COLON>" << endl;
                break;
            } else if (i == tokens[line].size() - 1) {
                cout << "ERROR: Missing colon after if statement at line: " << lookahead.line << endl;
                exit(1);
            }
        }
        
        //Add as childs next lines until end block found
        line ++;
        lookahead = tokens[line][tokenIndex];
        while (lookahead.type != BLOCK_END) {
            linesToSkip ++;
            ASTNode statement = parseStatement(tokens, line, tokenIndex, lookahead);
            ifStatement.children.push_back(statement);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + statement.children.size();
            if (lastElement.type == BLOCK_END) {
                if (DEBUG) cout << endl;
                break;
            }
            lookahead = tokens[line][tokenIndex];
        }
        
        //If next token is Else, add else as expression
        lookahead = tokens[line][tokenIndex];
        if (lookahead.type == ELSE) {
            if (DEBUG) cout << "<ELSE>";
            ASTNode elseStatement = ASTNode();
            elseStatement.type = AST_FUNCTION_CALL;
            elseStatement.line = lookahead.line;
            elseStatement.symbol = lookahead.value;

            if (tokens[line][tokenIndex+1].type != COLON) {
                cout << "ERROR: Missing colon after else statement at line: " << lookahead.line << endl;
                exit(1);
            }
            if (DEBUG) cout <<"<COLON>" << endl;

            line ++;
            linesToSkip ++;
            lookahead = tokens[line][tokenIndex];
            while (lookahead.type != BLOCK_END) {
                linesToSkip ++;
                if(line > tokens.size()) break;
                ASTNode statement = parseStatement(tokens, line, tokenIndex, lookahead);
                elseStatement.children.push_back(statement);
                token lastElement = tokens[line][tokens[line].size()-1];
                line += 1 + statement.children.size();
                if (lastElement.type == BLOCK_END) break;
                if (tokens.size() > line) {
                    lookahead = tokens[line][tokenIndex];
                }
            }

            ifStatement.children.push_back(elseStatement);
        }
        
        return ifStatement;
    }

    //<FUNCTION_DEFINITION>
    if (lookahead.type == FUNCTION_DEFINITION) {
        if (DEBUG) cout << "<DEF>";

        ASTNode function = ASTNode();
        function.type = AST_FUNCTION;
        function.line = lookahead.line;

        //Get funcion name
        int tokenIncrement = 1;
        lookahead = tokens[line][tokenIndex+tokenIncrement];
        if (lookahead.type != IDENTIFIER) {
            cout << "ERROR: no name found for function defnition at line: " << lookahead.line << endl;
            exit(1);
        }
        function.symbol = lookahead.value;

        //Get parameters
        tokenIncrement ++;
        lookahead = tokens[line][tokenIndex+tokenIncrement];
        while (lookahead.type != COLON) {
            if (lookahead.type == IDENTIFIER) {
                ASTNode parameter = ASTNode();
                parameter.type = AST_PARAMETER;
                parameter.symbol = lookahead.value;
                parameter.line = lookahead.line;
                function.parameters.push_back(parameter);
            } else if (lookahead.type == OPEN_PARENTHESES) {
                if (DEBUG) cout << "<OPEN_PARENTHESES>";
            } else if (lookahead.type == CLOSE_PARENTHESES) {
                if (DEBUG) cout << "<OPEN_PARENTHESES>";
            }
            tokenIncrement ++;
            lookahead = tokens[line][tokenIndex+tokenIncrement];
        }
        if (DEBUG) cout << "<COLON>" << endl;
        
        line ++;
        linesToSkip ++;
        lookahead = tokens[line][tokenIndex];
        while (lookahead.type != BLOCK_END) {
            linesToSkip ++;
            ASTNode funcStatement = parseStatement(tokens, line, tokenIndex, lookahead);
            function.children.push_back(funcStatement);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + funcStatement.children.size();
            if (lastElement.type == BLOCK_END) break;
            if (tokens.size() > line) {
                lookahead = tokens[line][tokenIndex];
            }
        }
        return function;
    }

    //<BLOCK_END>
    if (lookahead.type == BLOCK_END) {
        if (tokens[line].size() > 1) {
            return parseStatement(tokens, line, tokenIndex+1, tokens[line][tokenIndex+1]);
        } else {
            return ASTNode();
        }
    }

    //<RETURN>
    if (lookahead.type == RETURN) {
        if (DEBUG) cout << "<RETURN>";

        ASTNode returnNode = ASTNode();
        returnNode.type = AST_RETURN;
        returnNode.line = lookahead.line;

        lookahead = tokens[line][tokenIndex+1];
        returnNode.expression.push_back(parseStatement(tokens, line, tokenIndex+1, lookahead));

        if (DEBUG) cout << endl;

        return returnNode;
    }
    
    ASTNode error = ASTNode();
    error.type = AST_ERROR;
    error.value = "Unrecognized statement";
    return error;
}

//DEBUGGING
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
void interpret(ASTNode program) {
    for (int i=0; i<program.children.size(); i++) {
        traverse(program.children[i]);
    }
}

Symbol traverse(ASTNode node) {
    if (DEBUG) cout << "Traversing node of type " << node.type << endl;
    unordered_map<string, Symbol> variables2 = variables;

    if (node.type == AST_ASSIGMENT) {

        Symbol symbol = Symbol();
        string symbolName = node.symbol;
        symbol.line = node.line;

        if (node.expression.size() == 0) cout << "ERROR: Assigment without expression at line: " << node.line << endl;

        symbol = traverse(node.expression[0]);
        variables[symbolName] = symbol;

        return symbol;
    }

    if (node.type == AST_FUNCTION) {
        Symbol function = Symbol();
        string functionName = node.symbol;
        function.function = node;
        function.line = node.line;
        function.type = "function";
        variables[functionName] = function;
    }

    if (node.type == AST_FUNCTION_CALL) {
        if (node.symbol == "print") {
            if (node.expression.size() == 0) cout << "ERROR: Print without expression at line: " << node.line << endl;
            for (int i=0; i<node.expression[0].expression.size(); i++) {
                Symbol toPrint = traverse(node.expression[0].expression[i]);
                if (toPrint.type == AST_NUMBER) cout << toPrint.integer << " ";
                if (toPrint.type == AST_STRING) cout << toPrint.str << " ";
                if (toPrint.type == AST_LIST) {
                    cout << "[";
                    for (int i=0; i<toPrint.list.size(); i++) {
                        cout << toPrint.list[i];
                        if (i != toPrint.list.size() - 1) cout << ", ";
                    }
                    cout << "] ";
                }
            }

            cout << endl;
        }

        if (node.symbol == "len") {
            if (node.expression.size() == 0) cout << "ERROR: Len without expression at line:" << node.line << endl;
            Symbol list = traverse(node.expression[0]);
            if (list.type != AST_LIST) { 
                cout << "ERROR: Len called on non list at line: " << node.line << endl;
                exit(1);
            }
            Symbol length = Symbol();
            length.type = AST_NUMBER;
            length.integer = list.list.size();
            length.line = node.line;
            return length;
        }

        if (node.symbol == "if") {
            Symbol condition = traverse(node.parameters[0]);
            
            if (condition.boolean) {
                //If condition is true
                for (int i = 0; i < node.children.size(); i++) {
                    traverse(node.children[i]);
                }
            } else if (node.expression.size() > 0) {
                //if condition is false
                traverse(node.expression[0]);
            }
        }

        if (node.symbol == "else") {
            for (int i = 0; i < node.children.size(); i++) {
                traverse(node.children[i]);
            }
        }

        if (variables.find(node.symbol) != variables.end()) {
            ASTNode function = variables[node.symbol].function;

            //Get parameters
            for (int i = 0; i < function.parameters.size(); i++) {
                string parameterName = function.parameters[i].symbol;
                Symbol parameterValue = traverse(node.parameters[i]);
                variables[parameterName] = parameterValue;
            }

            //Execute function
            for (int i = 0; i < function.children.size(); i++) {
                if (function.children[i].type == AST_RETURN) {
                    return traverse(function.children[i]);
                }
                traverse(function.children[i]);
            }
            return Symbol();
        }
    }

    if (node.type == AST_OPERATION) {
        Symbol term1 = traverse(node.parameters[0]);
        Symbol term2 = traverse(node.parameters[1]);

        if (term1.type != term2.type) {
            cout << "ERROR: cannot operate " << term1.type << " with " << term2.type << ". At line: " << node.line << endl;
            exit(1);
        }

        Symbol result = Symbol();
        result.line = node.line;

        //SUM
        if (node.value == "+") {
            if (term1.type == AST_LIST && term2.type == AST_LIST) {
                Symbol newList = Symbol();
                newList.type = AST_LIST;
                newList.line = node.line;
                vector<int> list;
                for (int i = 0; i < term1.list.size(); i++) {
                    list.push_back(term1.list[i]);
                }
                for (int i = 0; i < term2.list.size(); i++) {
                    list.push_back(term2.list[i]);
                }
                newList.list = list;
                return newList;
            }

            result.type = AST_NUMBER;
            result.integer = term1.integer + term2.integer;
            return result;
        }

        //COMPARISON
        if (node.value == "==") {
            result.type = AST_BOOLEAN;
            if (term1.type == AST_NULL) {
                result.boolean = false;
            } else if (term2.type == AST_NULL) {
                result.boolean = false;
            } else {
                result.boolean = term1.integer == term2.integer;
            }
            return result;
        }

        //GREATER THAN
        if (node.value == ">") {
            result.type = AST_BOOLEAN;
            result.boolean = term1.integer > term2.integer;
            return result;
        }

        //LESS THAN
        if (node.value == "<") {
            result.type = AST_BOOLEAN;
            result.boolean = term1.integer < term2.integer;
            return result;
        }

        //GREATER THAN OR EQUAL
        if (node.value == ">=") {
            result.type = AST_BOOLEAN;
            result.boolean = term1.integer >= term2.integer;
            return result;
        }

        //LESS THAN OR EQUAL
        if (node.value == "<=") {
            result.type = AST_BOOLEAN;
            result.boolean = term1.integer <= term2.integer;
            return result;
        }

        //DIFFERENT
        if (node.value == "!=") {
            result.type = AST_BOOLEAN;
            result.boolean = term1.integer != term2.integer;
            return result;
        }

        cout << "ERROR: Operation not implemented at line: " << node.line << endl;
        exit(0);
    }

    if (node.type == AST_EXPRESSION) {
        return traverse(node.expression[0]);
    }

    if (node.type == AST_NUMBER) {
        Symbol symbol = Symbol();
        symbol.type = "NUMBER";
        symbol.integer = stoi(node.value);
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == AST_STRING) {
        Symbol symbol = Symbol();
        symbol.type = "STRING";
        symbol.str = node.value;
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == AST_BOOLEAN) {
        Symbol symbol = Symbol();
        symbol.type = "BOOLEAN";
        symbol.boolean = node.value == "true";
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == AST_LIST) {
        Symbol symbol = Symbol();
        symbol.type = "LIST";
        symbol.line = node.line;
        for (int i=0; i<node.expression.size(); i++) {
            Symbol listElement = traverse(node.expression[i]);
            symbol.list.push_back(listElement.integer);
        }
        return symbol;
    }

    if (node.type == AST_NULL) {
        Symbol symbol = Symbol();
        symbol.type = "NULL";
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == AST_LIST_INDEX) {
        if (variables.find(node.symbol) != variables.end()) {
            Symbol symbol = variables[node.symbol];
            if (node.expression.size() > 0) {
                Symbol parameter = traverse(node.parameters[0]);
                Symbol expression = traverse(node.expression[0]);
                symbol.list[parameter.integer] = expression.integer;
                return symbol;
            } else {
                Symbol result = Symbol();
                result.type = AST_NUMBER;
                result.line = node.line;
                result.integer = symbol.list[traverse(node.parameters[0]).integer];
                return result;
            }
        } else {
            cout << "ERROR: Variable " << node.symbol << " not found at line: " << node.line << endl;
            exit(1);
        }
    }

    if (node.type == AST_RETURN) {
        return traverse(node.expression[0]);
    }

    if (node.type == AST_IDENTIFIER) {
        if(variables.find(node.symbol) != variables.end()) {
            return variables[node.symbol];
        } else {
            cout << "ERROR: Variable " << node.symbol << " not found at line: " << node.line << endl;
            exit(1);
        }
    }
    return Symbol();
}