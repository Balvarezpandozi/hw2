# hw2
COSC 4315 - Programming Languages and Paradigms - Homework 2

## Instructions:
    

## Specifications:
### Grammar:
<PROGRAM> -> 
    | <ASSIGMENT>* 
    | <FUNCTION_CALL>*
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