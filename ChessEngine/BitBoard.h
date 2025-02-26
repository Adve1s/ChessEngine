#pragma once
#include <cassert>
#include <string>
#include <gsl/narrow>
#include "types.h"

namespace chess {

	void init();
	void initSquareDistance();
	void initBetweenThroughBB();
	void initAttackBB();

	// Constants for file masks
	constexpr Bitboard FILE_MASK_A = 0x0101010101010101ULL;
	constexpr Bitboard FILE_MASK_B = FILE_MASK_A << 1;
	constexpr Bitboard FILE_MASK_C = FILE_MASK_A << 2;
	constexpr Bitboard FILE_MASK_D = FILE_MASK_A << 3;
	constexpr Bitboard FILE_MASK_E = FILE_MASK_A << 4;
	constexpr Bitboard FILE_MASK_F = FILE_MASK_A << 5;
	constexpr Bitboard FILE_MASK_G = FILE_MASK_A << 6;
	constexpr Bitboard FILE_MASK_H = FILE_MASK_A << 7;

	// Constants for rank masks
	constexpr Bitboard RANK_MASK_1 = 0xFF;
	constexpr Bitboard RANK_MASK_2 = RANK_MASK_1 << (8 * 1);
	constexpr Bitboard RANK_MASK_3 = RANK_MASK_1 << (8 * 2);
	constexpr Bitboard RANK_MASK_4 = RANK_MASK_1 << (8 * 3);
	constexpr Bitboard RANK_MASK_5 = RANK_MASK_1 << (8 * 4);
	constexpr Bitboard RANK_MASK_6 = RANK_MASK_1 << (8 * 5);
	constexpr Bitboard RANK_MASK_7 = RANK_MASK_1 << (8 * 6);
	constexpr Bitboard RANK_MASK_8 = RANK_MASK_1 << (8 * 7);

	extern std::array<std::array<Bitboard, SQUARE_NB>, SQUARE_NB> g_betweenBB;
	extern std::array<std::array<Bitboard, SQUARE_NB>, SQUARE_NB> g_throughBB;
	extern std::array<std::array<Bitboard, SQUARE_NB>, PIECE_TYPE_NB> g_pseudoAttacks;
	extern std::array<std::array<Bitboard, SQUARE_NB>, COLOR_NB> g_pawnAttacks;

	extern std::array<std::array<uint8_t, SQUARE_NB>, SQUARE_NB> g_squareDistance;

	Bitboard insideBoard(Square square,int step);

	// Helper to determine the direction between two squares
	Direction getDirection(Square from, Square to);

	template<Color C>
	constexpr Bitboard pawnAttack(const Square square) {
		return C == WHITE ? insideBoard(square, NORTH_WEST) | insideBoard(square, NORTH_EAST)
			: insideBoard(square, SOUTH_WEST) | insideBoard(square, SOUTH_EAST);
	}

	template<typename T>
	int distance(const Square x, const Square y) noexcept {
		if constexpr (std::is_same_v<T, File>) {
			return abs(fileOf(x) - fileOf(y));
		}
		else if constexpr (std::is_same_v<T, Rank>) {
			return abs(rankOf(x) - rankOf(y));
		}
		else {
			// Disable warning about potential array bounds check
			// This is safe because all callers provide valid square values
			// and we cant afford to at() because function is used too often at run time
#pragma warning(push)
#pragma warning(disable: 26446)
#pragma warning(disable: 26482)
			return g_squareDistance[x][y];
#pragma warning(pop)
		}
	}

	Bitboard pawnAttack(Color color, Square square);
	// Core bitboard operations
	inline void setBit(Bitboard& board,const Square square) {
		assert(isSquare(square));
		board |= (1ULL << square);
	}

	inline void clearBit(Bitboard& board,const Square square) {
		assert(isSquare(square));
		board &= ~(1ULL << square);
	}

	// Query methods
	constexpr bool isSquareEmpty(const Bitboard board, const Square square) {
		assert(isSquare(square));
		return !(board & (1ULL << square));
	}

	// Utility methods for bitboard manipulation
	constexpr Bitboard squareToBB(const Square square) {
		assert(isSquare(square));
		return 1ULL << square;
	}

	// Get the least significant bit position (first set bit)
	inline Square lsb(const Bitboard board) noexcept {
		assert(board);  // Fail fast if empty
		unsigned long index;
		_BitScanForward64(&index, board);
		return static_cast<Square>(index);
	}

	// Get the most significant bit position (last set bit)
	inline Square msb(const Bitboard board) noexcept {
		assert(board);  // Fail fast if empty
		unsigned long index;
		_BitScanReverse64(&index, board);
		return static_cast<Square>(index);
	}

	// Get and clear the least significant bit
	inline Square popLsb(Bitboard& b) noexcept {
		assert(b);                    // Fail fast if empty
		const auto s = lsb(b);  // Reuse lsb() function
		// (x & (x-1)) clears the least significant set bit
		b &= b - 1;                   // Clear LSB
		return s;
	}

	inline int popCount(const Bitboard board) noexcept {
		return gsl::narrow_cast<int>(__popcnt64(board));
	}

	// Debug methods
	void printBitBoard(Bitboard board);

	// Helper functions - declared here, defined in cpp file
	std::string squareToString(Square square);
	int stringToSquare(const std::string& squareStr);
}