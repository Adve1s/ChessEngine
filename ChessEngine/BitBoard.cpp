#include "BitBoard.h"
#include <iostream>

namespace chess {

	Bitboard g_betweenBB[SQUARE_NB][SQUARE_NB];
	Bitboard g_throughBB[SQUARE_NB][SQUARE_NB];
	Bitboard g_pseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
	Bitboard g_pawnAttacks[COLOR_NB][SQUARE_NB];
	uint8_t g_squareDistance[SQUARE_NB][SQUARE_NB];

	void init() {
		for (Square sq1 = A1; sq1 < SQUARE_NB; ++sq1) 
			for (Square sq2 = A1; sq2 < SQUARE_NB; ++sq2) 
				g_squareDistance[sq1][sq2] = std::max(distance<File>(sq1, sq2),distance<Rank>(sq1, sq2));

		for (Square sq = A1; sq < SQUARE_NB; ++sq) {
			const File f = fileOf(sq);
			const Rank r = rankOf(sq);
			g_pseudoAttacks[ROOK][sq] = (FILE_MASK_A << f | RANK_MASK_1 << (8 * r)) & ~squareToBB(sq);
			for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
			{
				Square temp = sq;
				while (const Bitboard next = insideBoard(temp, d)) {
					g_pseudoAttacks[BISHOP][sq] |= next;
					temp = Square(temp + d);
				}
			}

			g_pseudoAttacks[QUEEN][sq] |= g_pseudoAttacks[BISHOP][sq]| g_pseudoAttacks[ROOK][sq];

			for (const int step : {-17, -15, -10, -6, 6, 10, 15, 17})
				g_pseudoAttacks[KNIGHT][sq] |= insideBoard(sq,step);

			for (const int step : {-9, -8, -7, -1, 1, 7, 8, 9})
				g_pseudoAttacks[KING][sq] |= insideBoard(sq, step);

			g_pawnAttacks[WHITE][sq] |= pawnAttack<WHITE>(sq);
			g_pawnAttacks[BLACK][sq] |= pawnAttack<BLACK>(sq);
		}
	}

	Bitboard insideBoard(const Square square, const int step) {
		Square to = Square(square + step);
		if (!isSquare(to)) {
			return Bitboard(0);
		}
		if (distance<Square>(square, to) <= 2) {
			return squareToBB(to);
		}
		return Bitboard(0);
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
	std::string squareToString(const Square square) {
		// Check if it's a valid square
		if (square == NO_SQUARE)
			return "none";

		// Convert square to file and rank
		File file = fileOf(square);   // Gets 0-7 for files a-h
		Rank rank = rankOf(square);   // Gets 0-7 for ranks 1-8

		// Create the string:
		// - File is converted to char by adding 'a' (e.g., 0 -> 'a', 1 -> 'b')
		// - Rank is converted to char by adding '1' (e.g., 0 -> '1', 1 -> '2')
		std::string result;
		result += char('a' + file);  // Convert file number to letter
		result += char('1' + rank);  // Convert rank number to digit

		return result;
	}
}