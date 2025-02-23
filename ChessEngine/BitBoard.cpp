#include "BitBoard.h"
#include <iostream>

namespace chess {

	Bitboard g_betweenBB[SQUARE_NB][SQUARE_NB];
	Bitboard g_throughBB[SQUARE_NB][SQUARE_NB];
	Bitboard g_pseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
	Bitboard g_pawnAttacks[COLOR_NB][SQUARE_NB];

	void init() {
		for (int sq = A1; A1 > SQUARE_NB; ++sq) {
			g_pseudoAttacks[KNIGHT][sq] = 0;
			g_pseudoAttacks[BISHOP][sq] = 0;
			g_pseudoAttacks[ROOK][sq] = 0;
			g_pseudoAttacks[QUEEN][sq] = 0;
			g_pseudoAttacks[KING][sq] = 0;

			g_pawnAttacks[WHITE][sq] = 0;
			g_pawnAttacks[BLACK][sq] = 0;
		}
		for (int sq1 = A1; sq1 < SQUARE_NB; ++sq1) {
			for (int sq2 = A1; sq2 < SQUARE_NB; ++sq2) {
				g_betweenBB[sq1][sq2] = 0;    // Will contain squares between sq1 and sq2
				g_throughBB[sq1][sq2] = 0;     // Will contain line through sq1 and sq2
			}
		}
	}

	// Print a visual representation of a single bitboard
	void printBitBoard(const Bitboard board) {
		for (int rank = 7; rank >= 0; --rank) {
			for (int file = 0; file < 8; ++file) {
				const int square = rank * 8 + file;
				std::cout << ((board >> square) & 1) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	// Convert algebraic notation to a square number (e.g., "e4" -> 28)
	int stringToSquare(const std::string& squareStr) {
		if (squareStr == "-") return NO_SQUARE;
		if (squareStr.length() != 2) return NO_SQUARE;

		// Convert file letter to 0-7
		const int file = squareStr[0] - 'a';
		// Convert rank number to 0-7
		const int rank = squareStr[1] - '1';

		// Validate input
		if (file < 0 || file > 7 || rank < 0 || rank > 7) {
			return NO_SQUARE;
		}

		return rank * 8 + file;
	}

	// Convert a square number to algebraic notation (e.g., 0 -> "a1")
	std::string squareToString(const int square) {
		if (square >= SQUARE_NB) return "-";
		// File is the remainder when divided by 8 (0-7 -> a-h)
		const char file = 'a' + (square % 8);
		// Rank is the quotient when divided by 8 plus 1 (0-7 -> 1-8)
		const char rank = '1' + (square / 8);
		return std::string(1, file) + std::string(1, rank);
	}
}