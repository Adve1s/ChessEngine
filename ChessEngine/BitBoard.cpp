#include "BitBoard.h"
#include <iostream>
#include <array>

namespace chess {

	std::array<std::array<Bitboard, SQUARE_NB>, SQUARE_NB> g_betweenBB;
	std::array<std::array<Bitboard, SQUARE_NB>, SQUARE_NB> g_throughBB;
	std::array<std::array<Bitboard, SQUARE_NB>, PIECE_TYPE_NB> g_pseudoAttacks;
	std::array<std::array<Bitboard, SQUARE_NB>, COLOR_NB> g_pawnAttacks;
	std::array<std::array<uint8_t, SQUARE_NB>, SQUARE_NB> g_squareDistance;

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
				g_squareDistance.at(sq1).at(sq2) = static_cast<uint8_t>(std::max(distance<File>(sq1, sq2), distance<Rank>(sq1, sq2)));
	}

	void initBetweenThroughBB()
	{
		// Define a local function to process a ray from a square in a given direction
		auto processRayFromSquare = [](Square sq1, Square sq2, Direction d) {
			Square temp = sq1;
			while (const Bitboard next = insideBoard(temp, d)) {
				g_throughBB.at(sq1).at(sq2) |= next;
				temp = static_cast<Square>(temp + d);
			}
			};
		for (Square sq1 = A1; sq1 < SQUARE_NB; ++sq1) {
			for (Square sq2 = A1; sq2 < SQUARE_NB; ++sq2) {
				const int dist = distance<Square>(sq1, sq2);
				g_throughBB.at(sq1).at(sq2) |= squareToBB(sq1);
				if (!dist) {
					continue;
				}
				std::array<Direction, 2> dirs;
				dirs.at(0) = getDirection(sq1, sq2);
				dirs.at(1) = getDirection(sq2, sq1);
				if (dirs.at(0) == Direction(0)) {
					g_throughBB.at(sq1).at(sq2) = Bitboard{ 0 };
					g_betweenBB.at(sq1).at(sq2) |= squareToBB(sq2);
					continue;
				}
				Square temp = sq1;
				for (int num = 0; num < dist - 1; ++num) {
					g_betweenBB.at(sq1).at(sq2) |= insideBoard(temp, dirs.at(0));
					temp = static_cast<Square>(temp + dirs.at(0));
				}
				for (const auto& d : dirs) {
					processRayFromSquare(sq1, sq2, d);
				}
			}
		}
	}

	void initAttackBB()
	{
		for (Square sq = A1; sq < SQUARE_NB; ++sq) {
			const File f = fileOf(sq);
			const Rank r = rankOf(sq);
			g_pseudoAttacks.at(ROOK).at(sq) = (FILE_MASK_A << f | RANK_MASK_1 << (8 * r)) & ~squareToBB(sq);
			for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
			{
				Square temp = sq;
				while (const Bitboard next = insideBoard(temp, d)) {
					g_pseudoAttacks.at(BISHOP).at(sq) |= next;
					temp = static_cast<Square>(temp + d);
				}
			}

			g_pseudoAttacks.at(QUEEN).at(sq) |= g_pseudoAttacks.at(BISHOP).at(sq) | g_pseudoAttacks.at(ROOK).at(sq);

			for (const int step : {-17, -15, -10, -6, 6, 10, 15, 17})
				g_pseudoAttacks.at(KNIGHT).at(sq) |= insideBoard(sq, step);

			for (const int step : {-9, -8, -7, -1, 1, 7, 8, 9})
				g_pseudoAttacks.at(KING).at(sq) |= insideBoard(sq, step);

			g_pawnAttacks.at(WHITE).at(sq) |= pawnAttack<WHITE>(sq);
			g_pawnAttacks.at(BLACK).at(sq) |= pawnAttack<BLACK>(sq);
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
	int stringToSquare(const std::string& squareStr)
	{
		if (squareStr == "-") return NO_SQUARE;
		if (squareStr.length() != 2) return NO_SQUARE;

		// Convert file letter to 0-7
		const int file = squareStr.at(0) - 'a';
		// Convert rank number to 0-7
		const int rank = squareStr.at(1) - '1';

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