#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "treeNode.h"
#include "token.h"
#include "parser.cpp"
#include <string>
#include <stack>
#include <vector>

int main()
{
    // Input string to tokenize
    std::string input = "let x = 10;\nfn y => (y + 20);";

    // Create a lexer instance
    lexer lexer;

    // Tokenize the input string
    lexer.tokenize(input);

    // Get the vector of tokens
    std::vector<token> tokens = lexer.getTokens();

    // Print out the tokens
    for (const token &t : tokens)
    {
        std::cout << "Type: " << t.tokType << ", Value: " << t.tokValue << ", Offset: " << t.offset << std::endl;
    }

    // Create a parser instance with the lexer
    parser parser(&lexer);

    // Parse the input tokens
    parser.parse();

    // Print the abstract syntax tree (AST)
    std::cout << "Abstract Syntax Tree (AST):" << std::endl;
    parser.printAST(parser.getTreeStack().top(), 0);

    return 0;
}
