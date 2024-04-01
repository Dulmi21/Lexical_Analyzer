g++ -std=c++11 -c token.cpp lexer.cpp main.cpp
g++ -o my_program token.o lexer.o main.o
./my_program