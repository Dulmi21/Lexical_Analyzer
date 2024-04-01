#include "lexer.h"
#include <cctype>    // For std::isdigit
#include <algorithm> // For std::find

Lexer::Lexer() {}

Lexer::~Lexer() {}

void Lexer::tokenize(const std::string &input)
{
    tokens.clear(); // Clear previous tokens

    int offset = 0; // Initialize offset

    size_t index = 0;
    while (index < input.size())
    {
        char c = input[index];

        if (isWhitespace(c))
        {
            // Skip whitespace
            index++;
            offset++;
        }
        else if (isIdentifier(c))
        {
            // Identify identifiers
            size_t start = index;
            while (index < input.size() && (isIdentifier(input[index]) || isDigit(input[index])))
            {
                index++;
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_IDENTIFIER, value, start);
        }
        else if (isDigit(c))
        {
            // Identify integers
            size_t start = index;
            while (index < input.size() && isDigit(input[index]))
            {
                index++;
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_INTEGER, value, start);
        }
        else if (isOperator(c))
        {
            // Identify operators
            size_t start = index;
            while (index < input.size() && isOperator(input[index]))
            {
                index++;
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_OPERATOR, value, start);
        }
        else if (c == '\'')
        {
            // Identify string literals
            size_t start = index;
            bool escaped = false;
            index++; // Skip opening quote
            offset++;
            while (index < input.size() && (input[index] != '\'' || escaped))
            {
                if (input[index] == '\\' && !escaped)
                {
                    escaped = true;
                }
                else
                {
                    escaped = false;
                }
                index++;
                offset++;
            }
            if (index < input.size())
            {
                index++; // Skip closing quote
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_STRING, value, start);
        }
        else if (isPunctuation(c))
        {
            // Identify punctuation
            addToken(TOK_PUNCTION, std::string(1, c), offset);
            index++;
            offset++;
        }
        else if (isEOL(c))
        {
            // Handle End-Of-Line
            index++;
            offset++;
        }
        else
        {
            // Invalid character, skip
            index++;
            offset++;
        }
    }
}

std::vector<token> Lexer::getTokens() const
{
    return tokens;
}

bool Lexer::isIdentifier(char c)
{
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c)
{
    return std::isdigit(c);
}

bool Lexer::isOperator(char c)
{
    // Define the list of valid operator symbols
    const std::string operators = "+-*/<>&.@/:=˜|$!#%^_[],{}\"‘?";
    return operators.find(c) != std::string::npos;
}

bool Lexer::isWhitespace(char c)
{
    return c == ' ' || c == '\t'; // Add more whitespace characters as needed
}

bool Lexer::isPunctuation(char c)
{
    // Check for valid punctuation characters
    return c == '(' || c == ')' || c == ';' || c == ',';
}

bool Lexer::isEOL(char c)
{
    // Check for End-Of-Line characters
    return c == '\n' || c == '\r';
}

void Lexer::addToken(tokenType type, const std::string &value, int offset)
{
    // Add a token to the token vector
    token t;
    t.tokType = type;
    t.tokValue = value;
    t.offset = offset;
    tokens.push_back(t);
}
