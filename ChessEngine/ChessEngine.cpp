// ChessEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "BitBoard.h"
#include "BitBoardTests.h"
#include "MagicBB.h"
#include "MagicBBTests.h"


int main()
{
	chess::init();
	chess::initMagicBitboards();
	chess::Bitboard ocup = 0;
	setBit(ocup, chess::C3);
	chess::printBitBoard(getBishopAttacks(chess::B2,ocup));
	chess::printBitBoard(getRookAttacks(chess::A3, ocup));
	chess::tests::runAllMagicBBTests();
	chess::tests::runAllBitBoardTests();
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
