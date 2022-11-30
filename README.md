# hw2
COSC 4315 - Programming Languages and Paradigms - Homework 2

## Instructions:
- To compile the file, use the following command:
            >>  g++ minipython.cpp -o topword  <<
## Specifications:
- The program uses three components (three passes on the input): A lexer that captures all symbols in the program, a parser that creates an abstract context tree and an interpreter that traverses and executes the code. Variables are stored in a map so that access time is O(1).
### Grammar:
<PROGRAM> -> 
    | <ASSIGMENT>* 
    | <FUNCTION_CALL>*
    | <EXPRESSION>*
    | <FUNCTION_DEFINITION>*
<ASSIGMENT> -> 
    <IDENTIFIER> <EQUALS> <EXPRESSION>
<EXPRESSION> ->     
    ( <NUMBER> 
    | <STRING> 
    | <OPERATION> 
    | <LIST> 
    | <LIST_INDEX>
    | <IF_STATEMENT> 
    | <EXPRESSION>) 
    <EXPRESSION>*
<OPERATION> -> Both terminals or nonterminals before and after the operator have to be equal or the second one can be an expression.
    |   (<IDENTIFIER>
    |   <NUMBER>
    |   <LIST>)
    <OPERATOR>
    (   <IDENTIFIER>
    |   <NUMBER>
    |   <LIST>
    |   <EXPRESSION>)
<NUMBER> -> [0-9]*
<STRING> -> "([a-zA-Z0-9]*)"
<LIST> -> [ ( <NUMBER> , )* <NUMBER> ]
<LIST_INDEX> -> <IDENTIFIER> [ <NUMBER> ]
<IF_STATEMENT> -> else statement does not have to be present
    if (<OPERATION>) { <BODY> } else { <BODY> }
<BODY> ->
    | <ASSIGMENT>* 
    | <FUNCTION_CALL>*