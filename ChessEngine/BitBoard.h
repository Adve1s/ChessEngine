#pragma once
#include <string>
#include "types.h"

namespace chess {

	void init();

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
	//extern uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];

	extern Bitboard g_betweenBB[SQUARE_NB][SQUARE_NB];
	extern Bitboard g_throughBB[SQUARE_NB][SQUARE_NB];
	extern Bitboard g_pseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
	extern Bitboard g_pawnAttacks[COLOR_NB][SQUARE_NB];

	// Core bitboard operations
	inline void setBit(Bitboard& board,const Square square) {
		board |= (1ULL << square);
	}

	inline void clearBit(Bitboard& board,const Square square) {
		board &= ~(1ULL << square);
	}

	// Query methods
	inline bool isSquareEmpty(const Bitboard board, const Square square) {
		return !(board & (1ULL << square));
	}

	// Utility methods for bitboard manipulation
	inline Bitboard squareToBitboard(const Square square) {
		return 1ULL << square;
	}

	// Get the least significant bit position (first set bit)
	inline int lsb(const Bitboard board) {
		if (board == 0) return -1;
		unsigned long index;
		_BitScanForward64(&index, board);

		return static_cast<int>(index);
	}

	// Get the most significant bit position (last set bit)
	inline int msb(const Bitboard board) {
		if (board == 0) return -1;
		unsigned long index;
		_BitScanReverse64(&index, board);

		return static_cast<int>(index);
	}

	// Get and clear the least significant bit
	inline int popLsb(Bitboard& board) {
		if (board == 0) return -1;

		unsigned long index;
		_BitScanForward64(&index, board);

		// Clear the least significant bit using a bitwise operation
		// (x & (x-1)) clears the least significant set bit
		board &= (board - 1);

		return static_cast<int>(index);
	}

	inline int popCount(const Bitboard board) {
		return static_cast<int>(__popcnt64(board));
	}

	// Debug methods
	void printBitBoard(Bitboard board);

	// Helper functions - declared here, defined in cpp file
	std::string squareToString(int square);
	int stringToSquare(const std::string& squareStr);

}