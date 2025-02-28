#pragma once
#include "Types.h"
#include <gsl/span>
// MagicBB.h - Magic bitboard implementation for fast sliding piece move generation

namespace chess {
	// Magic bitboard structure
	struct Magic {
		Bitboard mask = 0;      // Relevant occupancy mask for this square
		gsl::span<Bitboard> attacks;  // Pointer to attacks table for this square
		Bitboard magic = 0;     // Magic multiplier for perfect hash
		int shift= 0;          // Shift amount for the index

		// Calculate the attacks table index for a given occupancy
		inline unsigned int getIndex(Bitboard occupied) const noexcept {
			return gsl::narrow_cast<unsigned int>(((occupied & mask) * magic) >> shift);
		}
	};

	// Result of a magic number search
	struct MagicResult {
		Bitboard magic;  // The magic number found
		bool success;    // Whether a suitable magic was found
	};

	// Global arrays for magic bitboards
	extern std::array<Magic, SQUARE_NB> g_rookMagics;     // Rook magic data for each square
	extern std::array<Magic, SQUARE_NB> g_bishopMagics;   // Bishop magic data for each square
	extern std::array<Bitboard, 0x19000> g_rookTable;     // Rook attacks lookup table
	extern std::array<Bitboard, 0x1480> g_bishopTable;    // Bishop attacks lookup table

	// Function declarations
	void initMagics(PieceType piece);	// Initialize bishop or rook magics
	void initMagicBitboards();  // Initialize all the magic bitboard tables

	// Get attacks for sliding pieces using magic bitboards
	Bitboard getBishopAttacks(Square sq, Bitboard occupied);  // Bishop attacks
	Bitboard getRookAttacks(Square sq, Bitboard occupied);    // Rook attacks
	Bitboard getQueenAttacks(Square sq, Bitboard occupied);   // Queen attacks (bishop + rook)

	// Helper functions for initialization
	MagicResult findMagic(Square square, PieceType pieceType);  // Find a magic number
	Bitboard setOccupancy(int index, int bitsInMask, Bitboard mask);      // Generate occupancy variation

	// Functions to generate masks and attacks
	Bitboard generateBishopMask(Square sq);                     // Generate bishop relevant squares mask
	Bitboard generateRookMask(Square sq);                       // Generate rook relevant squares mask
	Bitboard generateBishopAttacks(Square sq, Bitboard occupied);  // Calculate bishop attacks directly
	Bitboard generateRookAttacks(Square sq, Bitboard occupied);    // Calculate rook attacks directly
}
