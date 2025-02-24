// ChessEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "Move.h"
#include "types.h"
#include "Position.h"
#include "BitBoard.h"
#include "BitBoardTests.h"

int main()
{
	init();
	tests::runAllTests();
	/***
	printBitBoard(g_throughBB[D4][F6]);
	printBitBoard(g_throughBB[F4][D4]);
	printBitBoard(g_throughBB[D4][D6]);
	printBitBoard(g_throughBB[D4][C1]);
	printBitBoard(g_throughBB[A1][A1]);

	printBitBoard(g_betweenBB[D4][F6]);
	printBitBoard(g_betweenBB[F4][D4]);
	printBitBoard(g_betweenBB[D4][D6]);
	printBitBoard(g_betweenBB[D4][C1]);
	printBitBoard(g_betweenBB[A1][A1]);

	printBitBoard(g_pawnAttacks[WHITE][A1]);
	printBitBoard(g_pawnAttacks[BLACK][A1]);
	printBitBoard(g_pseudoAttacks[KNIGHT][A1]);
	printBitBoard(g_pseudoAttacks[BISHOP][A1]);
	printBitBoard(g_pseudoAttacks[ROOK][A1]);
	printBitBoard(g_pseudoAttacks[QUEEN][A1]);
	printBitBoard(g_pseudoAttacks[KING][A1]);
	***/
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
