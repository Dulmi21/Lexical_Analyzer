#include "parser.h"
#include <iostream>
#include <vector>
#include <iterator>
#include "treeNode.h"


parser::parser(lexer *lex)
{
    this->lex = lex;
    nextToken = NULL;
}

parser::~parser()
{
}

void parser::read(const std::string &tokenStr)
{
    if (nextToken == NULL)
    {
        nextToken = lex->getNextToken();
    }

    if (nextToken->getValue() == tokenStr)
    {
        delete nextToken;
        nextToken = NULL;
    }
    else
    {
        std::cerr << "Error: Expected " << tokenStr << " but got " << nextToken->getValue() << std::endl;
        exit(1);
    }
}

void parser::buildTree(const std::string &nodeStr, int numChildNodes, int type)
{
    treeNode *newNode = new treeNode();
    treeNode *tempNode = new treeNode();
    newNode->nodeString = nodeStr;
    newNode->type = type;

    if (numChildNodes == 0)
    {
        treeStack.push(newNode);
    }
    if (treeStack.empty())
    {
        printf("We were asked to pop but the stack is empty\n");
    }
    else
    {
        while ((numChildNodes - 1) > 0)
        {
            if (!treeStack.empty())
            {
                tempNode = treeStack.top();
                treeStack.pop();
                if (treeStack.size() != 0)
                {
                    treeStack.top()->siblingNode = tempNode;
                }
                else if (treeStack.size() == 0)
                { // Stack cannot be empty here. We have one more element to pop before we can build/push the requested tree
                    printf("Parse Error: Empty Stack\n");
                    exit(0);
                }
                numChildNodes--;
            }
            else
            {
                printf("Stack size is less than numChildNodes. Abort.\n");
            }
        }
        tempNode = treeStack.top();
        newNode->childNode = tempNode;
        treeStack.pop();
    }
    treeStack.push(newNode);
}

bool parser::isKeyword(const std::string &val)
{
    if (val == "in" || val == "where" || val == "." || val == "aug" || val == "and" || val == "or" || val == "&" || val == "not" || val == "gr" || val == "ge" || val == "ls" || val == "le" || val == "eq" || val == "ne" || val == "+" || val == "-" || val == "*" || val == "/" || val == "**" || val == "@" || val == "within" || val == "rec" || val == "let" || val == "fn")
        return true;
    else
        return false;
}

void printAST(treeNode *node, int depth)
{
    // Function to print the abstract syntax tree(AST) void printAST(treeNode * node, int depth)
    {
        // Base case: If the node is null, return
        if (node == nullptr)
        {
            return;
        }

        // Print the node's string representation with indentation based on depth
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    std::cout << node->nodeString << std::endl;

    // Recursively print the children nodes with increased depth
    for (treeNode &child : *(node->childNode)) {
        printAST(&child, depth + 1);
    }
    }
}


void parser::parse()
{
    while (nextToken->tokType == TOK_DELETE)
    {
        nextToken = lex->getNextToken();
    }
    E();
    if (!treeStack.empty() && treeStack.size() != 1)
    {
        printf("Error: Stack not empty at the end of parsing\n");
        exit(0);
    }
    else if (lex->getNextToken()->tokType != TOK_EOF)
    {
        printf("Error: Parsing finished input still remaining\n");
        exit(0);
    }
    printf("Parsing Finished. AST Generated\n");
}

/* E    -> ’let’ D ’in’ E                          => ’let’
     -> ’fn’ Vb+ ’.’ E                          => ’lambda’
     ->  Ew;
*/
void parser::E()
{
    if (nextToken->tokValue == "let")
    {
        read("let");
        D();
        read("in");
        E();
        buildTree("let", 2, treeNode::LET);
    }
    else if (nextToken->tokValue == "fn")
    {
        read("fn");
        int n = 0;
        do
        {
            Vb();
            n++;
        } while (nextToken->tokValue == "(" || nextToken->tokType == TOK_IDENTIFIER);
        read(".");
        E();
        buildTree("lambda", n + 1, treeNode::LAMBDA);
    }
    else
    {
        Ew();
    }
}

/* Ew   -> T ’where’ Dr                            => ’where’
   -> T;*/
void parser::Ew()
{
    T();
    if (nextToken->tokValue == "where")
    {
        read("where");
        Dr();
        buildTree("where", 2, treeNode::WHERE);
    }
}

/* T    -> Ta ( ’,’ Ta )+                          => ’tau’
    -> Ta ;
 */
void parser::T()
{
    int n = 0;
    Ta();
    if (nextToken->tokValue == ",")
    {
        do
        {
            read(",");
            Ta();
            n++;
        } while (nextToken->tokValue == ",");
        buildTree("tau", n + 1, treeNode::TAU);
    }
}

/* Ta   -> Ta ’aug’ Tc                             => ’aug’
    -> Tc ;
 */
void parser::Ta()
{
    Tc();
    while (nextToken->tokValue == "aug")
    {
        read("aug");
        Tc();
        buildTree("aug", 2, treeNode::AUG);
    }
}

/* Tc   -> B ’->’ Tc ’|’ Tc                      => '->'
    -> B ;
 */
void parser::Tc()
{
    B();
    if (nextToken->tokValue == "->")
    {
        read("->");
        Tc();
        read("|");
        Tc();
        buildTree("->", 3, treeNode::TERNARY);
    }
}

/* B    -> B ’or’ Bt                               => ’or’
     -> Bt ;
  */
void parser::B()
{
    Bt();
    while (nextToken->tokValue == "or")
    {
        read("or");
        Bt();
        buildTree("or", 2, treeNode::OR);
    }
}

/*  Bt   -> Bt ’&’ Bs                               => ’&’
     -> Bs ;
  */
void parser::Bt()
{
    Bs();
    while (nextToken->tokValue == "&")
    {
        read("&");
        Bs();
        buildTree("&", 2, treeNode::AND_LOGICAL);
    }
}

/* Bs   -> ’not’ Bp                                => ’not’
     -> Bp ;
  */
void parser::Bs()
{
    if (nextToken->tokValue == "not")
    {
        read("not");
        Bp();
        buildTree("not", 1, treeNode::NOT);
    }
    else
    {
        Bp();
    }
}

/* Bp   -> A (’gr’ | ’>’ ) A                       => ’gr’
     -> A (’ge’ | ’>=’) A                       => ’ge’
     -> A (’ls’ | ’<’ ) A                       => ’ls’
     -> A (’le’ | ’<=’) A                       => ’le’
     -> A ’eq’ A                                => ’eq’
     -> A ’ne’ A                                => ’ne’
     -> A ;
  */
void parser::Bp()
{
    A();
    if (nextToken->tokValue == "gr" || nextToken->tokValue == ">")
    {
        if (nextToken->tokValue == "gr")
            read("gr");
        else if (nextToken->tokValue == ">")
            read(">");
        A();
        buildTree("gr", 2, treeNode::GR);
    }
    else if (nextToken->tokValue == "ge" || nextToken->tokValue == ">=")
    {
        if (nextToken->tokValue == "ge")
            read("ge");
        else if (nextToken->tokValue == ">=")
            read(">=");
        A();
        buildTree("ge", 2, treeNode::GE);
    }
    else if (nextToken->tokValue == "ls" || nextToken->tokValue == "<")
    {
        if (nextToken->tokValue == "ls")
            read("ls");
        else if (nextToken->tokValue == "<")
            read("<");
        A();
        buildTree("ls", 2, treeNode::LS);
    }
    else if (nextToken->tokValue == "le" || nextToken->tokValue == "<=")
    {
        if (nextToken->tokValue == "le")
            read("le");
        else if (nextToken->tokValue == "<=")
            read("<=");
        A();
        buildTree("le", 2, treeNode::LE);
    }
    else if (nextToken->tokValue == "eq")
    {
        read("eq");
        A();
        buildTree("eq", 2, treeNode::EQ);
    }
    else if (nextToken->tokValue == "ne")
    {
        read("ne");
        A();
        buildTree("ne", 2, treeNode::NE);
    }
}

/* A    -> A ’+’ At                                => ’+’
     -> A ’-’ At                                => ’-’
     ->   ’+’ At
     ->   ’-’ At                                => ’neg’
     -> At ;
  */
void parser::A()
{
    std::string treeStr;
    if (nextToken->tokValue == "+")
    {
        read("+");
        At();
    }
    else if (nextToken->tokValue == "-")
    {
        read("-");
        At();
        buildTree("neg", 1, treeNode::NEG);
    }
    else
    {
        At();
    }
    while (nextToken->tokValue == "+" || nextToken->tokValue == "-")
    {
        if (nextToken->tokValue == "+")
        {
            read("+");
            treeStr = "+";
        }
        else
        {
            read("-");
            treeStr = "-";
        }
        At();
        buildTree(treeStr, 2, treeStr == "+" ? treeNode::ADD : treeNode::SUBTRACT);
    }
}

/* At   -> At ’*’ Af                               => ’*’
     -> At ’/’ Af                               => ’/’
     -> Af ;
  */
void parser::At()
{
    std::string treeStr;
    Af();
    while (nextToken->tokValue == "*" || nextToken->tokValue == "/")
    {
        if (nextToken->tokValue == "*")
        {
            read("*");
            treeStr = "*";
        }
        else
        {
            read("/");
            treeStr = "/";
        }
        Af();
        buildTree(treeStr, 2, treeStr == "*" ? treeNode::MULTIPLY : treeNode::DIVIDE);
    }
}

/* Af   -> Ap ’**’ Af                              => ’**’
     -> Ap ;
  */
void parser::Af()
{
    Ap();
    if (nextToken->tokValue == "**")
    {
        read("**");
        Af();
        buildTree("**", 2, treeNode::EXPONENTIAL);
    }
}

/* Ap   -> Ap ’@’ ’<IDENTIFIER>’ R                 => ’@’
     -> R ;
  */
void parser::Ap()
{
    R();
    while (nextToken->tokValue == "@")
    {
        read("@");
        buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
        read(nextToken->tokValue);
        R();
        buildTree("@", 3, treeNode::AT);
    }
}

/* R    -> R Rn                                    => ’gamma’
     -> Rn ;
*/
void parser::R()
{
    Rn();
    while ((TOK_IDENTIFIER == nextToken->tokType || TOK_INTEGER == nextToken->tokType || TOK_STRING == nextToken->tokType || "(" == nextToken->tokValue || "false" == nextToken->tokValue || "true" == nextToken->tokValue || "nil" == nextToken->tokValue || "dummy" == nextToken->tokValue) && !isKeyword(nextToken->tokValue))
    {
        Rn();
        buildTree("gamma", 2, treeNode::GAMMA);
    }
}

/* Rn   -> ’<IDENTIFIER>’
     -> ’<INTEGER>’
     -> ’<STRING>’
     -> ’true’                                  => ’true’
     -> ’false’                                 => ’false’
     -> ’nil’                                   => ’nil’
     -> ’(’ E ’)’
     -> ’dummy’                                 => ’dummy’ ;
*/
void parser::Rn()
{
    if ("(" == nextToken->tokValue)
    {
        read("(");
        E();
        read(")");
    }
    else if (TOK_IDENTIFIER == nextToken->tokType || TOK_INTEGER == nextToken->tokType || TOK_STRING == nextToken->tokType)
    {
        if ("true" == nextToken->tokValue)
        {
            read("true");
            buildTree("<true>", 0, treeNode::TRUE);
        }
        else if ("false" == nextToken->tokValue)
        {
            read("false");
            buildTree("<false>", 0, treeNode::FALSE);
        }
        else if ("nil" == nextToken->tokValue)
        {
            read("nil");
            buildTree("<nil>", 0, treeNode::NIL);
        }
        else if ("dummy" == nextToken->tokValue)
        {
            read("dummy");
            buildTree("<dummy>", 0, treeNode::DUMMY);
        }
        else if (TOK_IDENTIFIER == nextToken->tokType)
        {
            buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
            read(nextToken->tokValue);
        }
        else if (TOK_STRING == nextToken->tokType)
        {
            buildTree(nextToken->tokValue, 0, treeNode::STRING);
            read(nextToken->tokValue);
        }
        else if (TOK_INTEGER == nextToken->tokType)
        {
            buildTree(nextToken->tokValue, 0, treeNode::INTEGER);
            read(nextToken->tokValue);
        }
    }
}

/* D    -> Da ’within’ D                           => ’within’
     -> Da ;
  */
void parser::D()
{
    Da();

    if (nextToken->tokValue == "within")
    {
        read("within");
        D();
        buildTree("within", 2, treeNode::WITHIN);
    }
}
/*     Da   -> Dr ( ’and’ Dr )+                        => ’and’
         -> Dr ;
  */
void parser::Da()
{
    int n = 0;
    Dr();
    while (nextToken->tokValue == "and")
    {
        read("and");
        Dr();
        n++;
    }
    if (n > 0)
        buildTree("and", n + 1, treeNode::AND);
}

/*      Dr   -> ’rec’ Db                                => ’rec’
         -> Db ;
  */
void parser::Dr()
{
    if (nextToken->tokValue == "rec")
    {
        read("rec");
        Db();
        buildTree("rec", 1, treeNode::REC);
    }
    else
    {
        Db();
    }
}

/* Db   -> Vl ’=’ E                              => ’=’
     -> ’<IDENTIFIER>’ Vb+ ’=’ E              => ’fcn_form’
     -> ’(’ D ’)’ ;
  */
void parser::Db()
{
    if (nextToken->tokValue == "(")
    {
        read("(");
        D();
        read(")");
    }
    else if (nextToken->tokType == TOK_IDENTIFIER)
    {
        // Since identifier type is common here, read it here now and consider it for build tree later.
        buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
        read(nextToken->tokValue);
        if (nextToken->tokValue == "," || nextToken->tokValue == "=")
        {
            Vl();
            read("=");
            E();
            buildTree("=", 2, treeNode::BINDING);
        }
        else
        {
            int n = 0;
            do
            {
                Vb();
                n++;
            } while (nextToken->tokValue == "(" || nextToken->tokType == TOK_IDENTIFIER);
            read("=");
            E();
            buildTree("function_form", n + 2, treeNode::FCN_FORM); // The identifier at the start of this function is included here as n + 2
        }
    }
}

/*    Vb   -> ’<IDENTIFIER>’
         -> ’(’ Vl ’)’
         -> ’(’ ’)’                                 => ’()’;
  */
void parser::Vb()
{
    if (nextToken->tokType == TOK_IDENTIFIER)
    {
        buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
        read(nextToken->tokValue);
    }
    else if (nextToken->tokValue == "(")
    {
        read("(");
        if (nextToken->tokValue == ")")
        {
            read(")");
            buildTree("()", 0, treeNode::PARANTHESES);
        }
        else if (nextToken->tokType == TOK_IDENTIFIER)
        {
            // Before getting into Vl, an identifier must be read
            // Vl expects its caller to do this.
            buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
            read(nextToken->tokValue);
            Vl();
            read(")");
        }
    }
}

//    Vl   -> ’<IDENTIFIER>’ list ’,’                 => ’,’?;
void parser::Vl()
{
    int n = 0;
    while (nextToken->tokValue == ",")
    {
        read(",");
        if (nextToken->tokType == TOK_IDENTIFIER)
        {
            buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
            read(nextToken->tokValue);
            n++;
        }
        else
        {
            printf("ERROR In Vl()\n");
        }
    }
    // n+1 for the identifier that was read before Vl was called.
    if (n > 0)
        buildTree(",", n + 1, treeNode::COMMA);
}

void printAST(treeNode *node, int depth)
{
    // Function to print the abstract syntax tree(AST) void printAST(treeNode * node, int depth)
    {
        // Base case: If the node is null, return
        if (node == nullptr)
        {
            return;
        }

        // Print the node's string representation with indentation based on depth
        for (int i = 0; i < depth; ++i)
        {
            std::cout << "  ";
        }
        std::cout << node->nodeString << std::endl;

        // Recursively print the children nodes with increased depth
        for (treeNode *child : node->childNode)
        {
            printAST(child, depth + 1);
        }
    }
}

std::stack<treeNode *> parser::getTreeStack() const
{
    return treeStack;
}
