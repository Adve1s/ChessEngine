#pragma once
#include "types.h"
#include "BitBoard.h" 

namespace chess {
	struct Magic {
		Bitboard mask;      // Relevant occupancy mask - these are the squares that matter for this piece at this position
		Bitboard* attacks;  // Pointer to the location in the attacks table where this square's data begins
		Bitboard magic;     // The "magic" multiplier that creates a perfect hash
		int shift;          // How many bits to shift right after multiplication

		// Calculate index into attacks table
		inline unsigned int getIndex(Bitboard occupied) const {
			// This is the core of the magic bitboard technique:
			// 1. Filter the occupied squares to only those that matter (the mask)
			// 2. Multiply by the magic number to create a unique pattern
			// 3. Shift right to compress into an index
			return static_cast<unsigned int>(((occupied & mask) * magic) >> shift);
		}
	};

	// Global arrays to store our magic data
	extern Magic g_rookMagics[SQUARE_NB];      // One entry per square for rooks
	extern Magic g_bishopMagics[SQUARE_NB];    // One entry per square for bishops
	extern Bitboard g_rookTable[0x19000];      // Table for rook attacks (size based on Stockfish)
	extern Bitboard g_bishopTable[0x1480];     // Table for bishop attacks (size based on Stockfish)

	// Function declarations
	void initMagicBitboards();  // Initialize all the magic bitboard tables

	// Core lookup functions you'll use during move generation
	Bitboard getBishopAttacks(Square sq, Bitboard occupied);  // Get bishop attacks with magic lookup
	Bitboard getRookAttacks(Square sq, Bitboard occupied);    // Get rook attacks with magic lookup
	Bitboard getQueenAttacks(Square sq, Bitboard occupied);   // Get queen attacks (bishop + rook)

	// Helper functions for initialization
	Bitboard findMagic(Square square, PieceType pieceType, int bits);
	Bitboard setOccupancy(int index, int bitsInMask, Bitboard mask);  // Helper to generate occupancy variations

	// Functions to generate masks and raw attacks (used during initialization)
	Bitboard generateBishopMask(Square sq);    // Generate the relevant squares mask for a bishop
	Bitboard generateRookMask(Square sq);      // Generate the relevant squares mask for a rook
	Bitboard generateBishopAttacks(Square sq, Bitboard occupied);  // Calculate bishop attacks directly
	Bitboard generateRookAttacks(Square sq, Bitboard occupied);    // Calculate rook attacks directly
}