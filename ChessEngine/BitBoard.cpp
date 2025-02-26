#include "BitBoard.h"
#include <iostream>

namespace chess {

	Bitboard g_betweenBB[SQUARE_NB][SQUARE_NB];
	Bitboard g_throughBB[SQUARE_NB][SQUARE_NB];
	Bitboard g_pseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
	Bitboard g_pawnAttacks[COLOR_NB][SQUARE_NB];
	uint8_t g_squareDistance[SQUARE_NB][SQUARE_NB];

	void init()
	{
		initSquareDistance();
		initBetweenThroughBB();
		initAttackBB();
	}

	void initSquareDistance()
	{
		for (Square sq1 = A1; sq1 < SQUARE_NB; ++sq1)
			for (Square sq2 = A1; sq2 < SQUARE_NB; ++sq2)
				g_squareDistance[sq1][sq2] = static_cast<uint8_t>(std::max(distance<File>(sq1, sq2), distance<Rank>(sq1, sq2)));
	}

	void initBetweenThroughBB()
	{
		for (Square sq1 = A1; sq1 < SQUARE_NB; ++sq1) {
			for (Square sq2 = A1; sq2 < SQUARE_NB; ++sq2) {
				const int dist = distance<Square>(sq1, sq2);
				g_throughBB[sq1][sq2] |= squareToBB(sq1);
				if (!dist) {
					continue;
				}
				Direction dirs[2] = { };
				dirs[0] = getDirection(sq1, sq2);
				dirs[1] = getDirection(sq2, sq1);
				if (dirs[0] == Direction(0)) {
					g_throughBB[sq1][sq2] = Bitboard{ 0 };
					g_betweenBB[sq1][sq2] |= squareToBB(sq2);
					continue;
				}
				Square temp = sq1;
				for (int num = 0; num < dist - 1; ++num) {
					g_betweenBB[sq1][sq2] |= insideBoard(temp, dirs[0]);
					temp = static_cast<Square>(temp + dirs[0]);
				}
				for (const auto& d : dirs) {
					temp = sq1;
					while (const Bitboard next = insideBoard(temp, d)) {
						g_throughBB[sq1][sq2] |= next;
						temp = static_cast<Square>(temp + d);
					}
				}
			}
		}
	}

	void initAttackBB()
	{
		for (Square sq = A1; sq < SQUARE_NB; ++sq) {
			const File f = fileOf(sq);
			const Rank r = rankOf(sq);
			g_pseudoAttacks[ROOK][sq] = (FILE_MASK_A << f | RANK_MASK_1 << (8 * r)) & ~squareToBB(sq);
			for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
			{
				Square temp = sq;
				while (const Bitboard next = insideBoard(temp, d)) {
					g_pseudoAttacks[BISHOP][sq] |= next;
					temp = static_cast<Square>(temp + d);
				}
			}

			g_pseudoAttacks[QUEEN][sq] |= g_pseudoAttacks[BISHOP][sq] | g_pseudoAttacks[ROOK][sq];

			for (const int step : {-17, -15, -10, -6, 6, 10, 15, 17})
				g_pseudoAttacks[KNIGHT][sq] |= insideBoard(sq, step);

			for (const int step : {-9, -8, -7, -1, 1, 7, 8, 9})
				g_pseudoAttacks[KING][sq] |= insideBoard(sq, step);

			g_pawnAttacks[WHITE][sq] |= pawnAttack<WHITE>(sq);
			g_pawnAttacks[BLACK][sq] |= pawnAttack<BLACK>(sq);
		}
	}

	Bitboard insideBoard(const Square square, const int step)
	{
		assert(isSquare(square));
		const auto to = static_cast<Square>(square + step);
		if (!isSquare(to)) {
			return Bitboard{ 0 };
		}
		if (distance<Square>(square, to) <= 2) {
			return squareToBB(to);
		}
		return Bitboard{ 0 };
	}

	// Helper to determine the direction between two squares
	Direction getDirection(const Square from,const Square to)
	{
		const int fileDiff = fileOf(to) - fileOf(from);
		const int rankDiff = rankOf(to) - rankOf(from);

		if (fileDiff == 0)
			return rankDiff > 0 ? NORTH : SOUTH;
		if (rankDiff == 0)
			return fileDiff > 0 ? EAST : WEST;
		if (abs(fileDiff) == abs(rankDiff)) {
			if (rankDiff > 0)
				return fileDiff > 0 ? NORTH_EAST : NORTH_WEST;
			else
				return fileDiff > 0 ? SOUTH_EAST : SOUTH_WEST;
		}
		return Direction{ 0 };
	}

	// Print a visual representation of a single bitboard
	void printBitBoard(const Bitboard board)
	{
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
	int stringToSquare(const std::string& squareStr) noexcept
	{
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
	std::string squareToString(const Square square)
	{
		assert(isSquare(square));
		// Convert square to file and rank
		const File file = fileOf(square);   // Gets 0-7 for files a-h
		const Rank rank = rankOf(square);   // Gets 0-7 for ranks 1-8

		// Create the string:
		// - File is converted to char by adding 'a' (e.g., 0 -> 'a', 1 -> 'b')
		// - Rank is converted to char by adding '1' (e.g., 0 -> '1', 1 -> '2')
		std::string result;
		result += gsl::narrow_cast<char>('a' + file);  // Convert file number to letter
		result += gsl::narrow_cast<char>('1' + rank);  // Convert rank number to digit

		return result;
	}
}