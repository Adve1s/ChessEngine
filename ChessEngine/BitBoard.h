#pragma once
#include <string>
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

	//extern uint8_t PopCnt16[1 << 16];

	extern Bitboard g_betweenBB[SQUARE_NB][SQUARE_NB];
	extern Bitboard g_throughBB[SQUARE_NB][SQUARE_NB];
	extern Bitboard g_pseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
	extern Bitboard g_pawnAttacks[COLOR_NB][SQUARE_NB];

	extern uint8_t g_squareDistance[SQUARE_NB][SQUARE_NB];

	Bitboard insideBoard(Square square,int step);

	// Helper to determine the direction between two squares
	Direction getDirection(Square from, Square to);

	template<Color C>
	constexpr Bitboard pawnAttack(const Square square) {
		return C == WHITE ? insideBoard(square, NORTH_WEST) | insideBoard(square, NORTH_EAST)
			: insideBoard(square, SOUTH_WEST) | insideBoard(square, SOUTH_EAST);
	}

	template<typename T>
	inline int distance(const Square x, const Square y) {
		if constexpr (std::is_same_v<T, File>) {
			return abs(fileOf(x) - fileOf(y));
		}
		else if constexpr (std::is_same_v<T, Rank>) {
			return abs(rankOf(x) - rankOf(y));
		}
		else {
			return g_squareDistance[x][y];
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
	inline Square lsb(const Bitboard board) {
		assert(board);  // Fail fast if empty
		unsigned long index;
		_BitScanForward64(&index, board);
		return Square(index);
	}

	// Get the most significant bit position (last set bit)
	inline Square msb(const Bitboard board) {
		assert(board);  // Fail fast if empty
		unsigned long index;
		_BitScanReverse64(&index, board);
		return Square(index);
	}

	// Get and clear the least significant bit
	inline Square popLsb(Bitboard& b) {
		assert(b);                    // Fail fast if empty
		const Square s = Square(lsb(b));  // Reuse lsb() function
		// (x & (x-1)) clears the least significant set bit
		b &= b - 1;                   // Clear LSB
		return s;
	}

	inline int popCount(const Bitboard board) {
		return static_cast<int>(__popcnt64(board));
	}

	// Debug methods
	void printBitBoard(Bitboard board);

	// Helper functions - declared here, defined in cpp file
	std::string squareToString(Square square);
	int stringToSquare(const std::string& squareStr);
}