#include "BitBoard.h"
#include <iostream>
#include <array>

// Bitboard.cpp - Implementation of bitboard functions

namespace chess {

	// Global lookup tables
	std::array<std::array<Bitboard, SQUARE_NB>, SQUARE_NB> g_betweenBB;
	std::array<std::array<Bitboard, SQUARE_NB>, SQUARE_NB> g_throughBB;
	std::array<std::array<Bitboard, SQUARE_NB>, PIECE_TYPE_NB> g_pseudoAttacks;
	std::array<std::array<Bitboard, SQUARE_NB>, COLOR_NB> g_pawnAttacks;
	std::array<std::array<uint8_t, SQUARE_NB>, SQUARE_NB> g_squareDistance;

	namespace bitboards {
		// Initialize all lookup tables
		void init()
		{
			initSquareDistance();
			initBetweenThroughBB();
			initAttackBB();
		}
	}

	// Initialize distance lookup table
	void initSquareDistance()
	{
		assert(distance<Rank>(A1,A3) == 2 && distance<File>(A1,H1) == 7);
		for (Square sq1 = A1; sq1 < SQUARE_NB; ++sq1)
			for (Square sq2 = A1; sq2 < SQUARE_NB; ++sq2)
				g_squareDistance.at(sq1).at(sq2) = static_cast<uint8_t>(std::max(distance<File>(sq1, sq2), distance<Rank>(sq1, sq2)));
	}

	// Initialize between and through bitboards
	void initBetweenThroughBB()
	{
		assert(insideBoard(A1, 1) && !insideBoard(A1, 27));
		assert(distance<Square>(A1, A3) == 2);
		assert(getDirection(A1, C3) == NORTH_EAST && getDirection(A1, C7) == 0);
		// Helper to process rays from a square
		auto processRayFromSquare = [](Square sq1, Square sq2, Direction d) {
			Square temp = sq1;
			while (const Bitboard next = insideBoard(temp, d)) {
				g_throughBB.at(sq1).at(sq2) |= next;
				temp = temp + d;
			}
			};
		for (Square sq1 = A1; sq1 < SQUARE_NB; ++sq1) {
			for (Square sq2 = A1; sq2 < SQUARE_NB; ++sq2) {
				const int dist = distance<Square>(sq1, sq2);
				g_throughBB.at(sq1).at(sq2) |= squareToBB(sq1);
				if (!dist) {
					continue; // Skip self-square
				}
				std::array<Direction, 2> dirs;
				dirs.at(0) = getDirection(sq1, sq2);
				dirs.at(1) = getDirection(sq2, sq1);
				if (dirs.at(0) == Direction(0)) {
					// Not on same line/diagonal
					g_throughBB.at(sq1).at(sq2) = Bitboard{ 0 };
					g_betweenBB.at(sq1).at(sq2) |= squareToBB(sq2);
					continue;
				}
				// Calculate squares between
				Square temp = sq1;
				for (int num = 0; num < dist - 1; ++num) {
					g_betweenBB.at(sq1).at(sq2) |= insideBoard(temp, dirs.at(0));
					temp = temp + dirs.at(0);
				}
				// Calculate rays through squares
				for (const auto& d : dirs) {
					processRayFromSquare(sq1, sq2, d);
				}
			}
		}
		// Verify everything worked
		assert(g_betweenBB.at(A1).at(C3) == squareToBB(B2) && g_throughBB.at(A1).at(A5) == FILE_MASK_A 
			&& g_betweenBB.at(A1).at(B3) == squareToBB(B3) && g_throughBB.at(A1).at(A1) == squareToBB(A1));
	}

	// Initialize attack patterns for all pieces
	void initAttackBB()
	{
		static_assert(fileOf(B2) == FILE_B && rankOf(B2) == RANK_2);
		assert(insideBoard(A1, 1) && !insideBoard(A1, 27));
		for (Square sq = A1; sq < SQUARE_NB; ++sq) {
			const File f = fileOf(sq);
			const Rank r = rankOf(sq);
			assert(0 <= f && 0 <= r && f < 8 && r < 8);
			// Rook attacks
			g_pseudoAttacks.at(ROOK).at(sq) = (FILE_MASK_A << f | RANK_MASK_1 << (8 * r)) & ~squareToBB(sq);
			// Bishop attacks
			for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
			{
				Square temp = sq;
				while (const Bitboard next = insideBoard(temp, d)) {
					g_pseudoAttacks.at(BISHOP).at(sq) |= next;
					temp = temp + d;
				}
			}
			// Queen = rook + bishop
			g_pseudoAttacks.at(QUEEN).at(sq) |= g_pseudoAttacks.at(BISHOP).at(sq) | g_pseudoAttacks.at(ROOK).at(sq);
			// Knight L-shaped moves
			for (const int step : {-17, -15, -10, -6, 6, 10, 15, 17})
				g_pseudoAttacks.at(KNIGHT).at(sq) |= insideBoard(sq, step);
			// King moves (one square in any direction)
			for (const int step : {-9, -8, -7, -1, 1, 7, 8, 9})
				g_pseudoAttacks.at(KING).at(sq) |= insideBoard(sq, step);
			// Pawn attacks
			g_pawnAttacks.at(WHITE).at(sq) |= pawnAttack<WHITE>(sq);
			g_pawnAttacks.at(BLACK).at(sq) |= pawnAttack<BLACK>(sq);
		}
		// Verify everything worked
		assert(popCount(g_pseudoAttacks.at(KING).at(A1)) == 3 && popCount(g_pseudoAttacks.at(KNIGHT).at(E4)) == 8 
			&& popCount(g_pseudoAttacks.at(ROOK).at(C3)) == 14 && popCount(pawnAttack<WHITE>(C3)) == 2);
	}

	// Helper to determine if step is possible for current square
	Bitboard insideBoard(const Square square, const int step)
	{
		assert(isSquare(square));
		assert(distance<Square>(A1, A3) == 2);
		const auto to = static_cast<Square>(square + step);
		if (!isSquare(to)) {
			return Bitboard{ 0 }; // Not a valid square
		}
		if (distance<Square>(square, to) <= 2) {
			return squareToBB(to); // Valid move
		}
		return Bitboard{ 0 }; // Wraps around the board
	}

	// Helper to determine the direction between two squares
	Direction getDirection(const Square from,const Square to)
	{
		assert(isSquare(from) && isSquare(to));
		static_assert(fileOf(B2) == FILE_B && rankOf(B2) == RANK_2);
		const int fileDiff = fileOf(to) - fileOf(from);
		const int rankDiff = rankOf(to) - rankOf(from);

		// Vertical
		if (fileDiff == 0)
			return rankDiff > 0 ? NORTH : SOUTH;
		// Horizontal
		if (rankDiff == 0)
			return fileDiff > 0 ? EAST : WEST;
		// Diagonal
		if (abs(fileDiff) == abs(rankDiff)) {
			if (rankDiff > 0)
				return fileDiff > 0 ? NORTH_EAST : NORTH_WEST;
			else
				return fileDiff > 0 ? SOUTH_EAST : SOUTH_WEST;
		}
		return Direction{ 0 }; // Not on same line
	}

	// Print bitboard to console (for debugging)
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

	// Convert string to square (e.g., "e4" -> 28)
	int stringToSquare(const std::string& squareStr)
	{
		if (squareStr == "-") return NO_SQUARE;
		if (squareStr.length() != 2) return NO_SQUARE;

		const int file = squareStr.at(0) - 'a';
		const int rank = squareStr.at(1) - '1';

		if (file < 0 || file > 7 || rank < 0 || rank > 7) {
			return NO_SQUARE;
		}

		return rank * 8 + file;
	}

	// Convert square to string (e.g., 0 -> "a1")
	std::string squareToString(const Square square)
	{
		assert(isSquare(square));
		static_assert(fileOf(B2) == FILE_B && rankOf(B2) == RANK_2);
		const File file = fileOf(square);
		const Rank rank = rankOf(square);

		assert(file>=0 && file<8 && rank>=0 && rank<8);
		std::string result;
		result += gsl::narrow_cast<char>('a' + file);
		result += gsl::narrow_cast<char>('1' + rank);

		return result;
	}
}