# NDR Lexer and Parser (NDR_LAP)

## Purpose
NDR_LAP is set of tools for creating programming languages.
Lexical analysis, text parsing, abstract syntax tree building, and tree traversal are all provided for within NDR_LAP.

The inspiration for the NDR_LAP library is the lex/yacc program (and other variations) used to create the C++, Portable C, AWK and several other languages.
The goal of this project is to provide a more straight forward tool for anyone interested in programming language construction.

## How To Use
The project is set up to be built using cmake.
After running cmake and make commands to build the project, the necessary libraries will be present within the "lib" folder and the header files will be present within the "include" folder.
These can be used within any c project created to access all of the features of NDR_LAP 

## Documentation
[Documentation for this project](documentation/html/index.html)




NDR Parser

The parser configuration works by first giving an ID followed immediately by a colon and then defining strings that are to be associated with the preceding ID
The parser then compares the file text with the parser definitions and builds a syntax tree representing the text found.

For Example
additionStatement: number + number

In this example, any two numbers with a '+' character in between them will be seen as an additionStatement

If there are multiple strings that should be associated with a given ID, then the string "\|" can be used to separate the values  

For Example
`multiplicationStatement: number x number  

			    \| number * number`

In this example, any two numbers with a 'x' character in between them will be seen as a multiplicationStatement as well as any two numbers with a '*' character in between them

Note: The spacing does not matter

`multiplicationStatement: number x number
			    \| number * number`

is equivalent to

`multiplicationStatement: 
	number x number			 \| number * number`
