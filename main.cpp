#include <iostream>
#include "lexer.h"

int main()
{
    // Input string to tokenize
    std::string input = "let x = 10;\nfn y => (y + 20);";

    // Create a lexer instance
    Lexer lexer;

    // Tokenize the input string
    lexer.tokenize(input);

    // Get the vector of tokens
    std::vector<token> tokens = lexer.getTokens();

    // Print out the tokens
    for (const token &t : tokens)
    {
        std::cout << "Type: " << t.tokType << ", Value: " << t.tokValue << ", Offset: " << t.offset << std::endl;
    }

    return 0;
}
