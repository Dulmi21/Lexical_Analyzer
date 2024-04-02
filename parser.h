#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <stack>
#include "lexer.h"
#include "treeNode.h" // Assuming you have a treeNode class for constructing the parse tree

class parser
{
public:
    lexer *lex;
    token *nextToken;
    std::stack<treeNode *> treeStack;

    // Helper methods for parsing different non-terminals
    void E();
    void Ew();
    void T();
    void Ta();
    void Tc();
    void B();
    void Bt();
    void Bs();
    void Bp();
    void A();
    void At();
    void Af();
    void Ap();
    void R();
    void Rn();
    void D();
    void Da();
    void Dr();
    void Db();
    void Vb();
    void Vl();

    // Other helper methods
    void parse();
    bool isKeyword(const std::string &value);
    void read(const std::string &tokenStr);
    void buildTree(const std::string &nodeStr, int numChildNodes, int type);
    void printAST(int depth, treeNode *node);
    std::string to_s(treeNode *node);


//void printAST(treeNode *node, int depth)
public:
    parser(lexer *lex);
    void printAST();
    virtual ~parser();
    std::stack<treeNode *> getTreeStack() const;
};

#endif /* PARSER_H */
