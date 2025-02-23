// ChessEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "Move.h"
#include "types.h"
#include "Position.h"
#include "BitBoard.h"

int main()
{
	init();
	Position pos1;
	Bitboard board = 0;
	setBit(board, A1);
	printBitBoard(board);
	std::cout << stringToSquare("a1") << "\n";
	std::cout << squareToString(A2) << "\n";
	Move move(B1,C3,KNIGHT);
	Move move2(B1, C3, KNIGHT);
	std::cout << (move == move2) << "\n";
	std::cout << move.toString() << "\n";
	return 0;
}

//"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" FEN string for starting position

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
