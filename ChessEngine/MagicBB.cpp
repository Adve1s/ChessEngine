#include "MagicBB.h"
#include <random>
#include <vector>
#include <array>
#include "BitBoard.h"

// MagicBB.cpp - Implementation of magic bitboard functionality

namespace chess {

	// Generate bishop mask (excludes edges and the source square)
	Bitboard generateBishopMask(const Square sq)
	{
		assert(isSquare(sq));
		Bitboard mask = 0;
		constexpr Bitboard edges = FILE_MASK_A | FILE_MASK_H | RANK_MASK_1 | RANK_MASK_8;
		// For each diagonal direction
		for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				if (edges & next)
					break; // Stop at board edge
				mask |= next;
				temp = static_cast<Square>(temp + d);
			}
		}
		assert(!(mask & squareToBB(sq)));
		return mask;
	}

	// Generate rook mask (excludes edges and the source square)
	Bitboard generateRookMask(const Square sq)
	{
		assert(isSquare(sq));
		Bitboard mask = 0;
		constexpr std::array<std::pair<Direction, Bitboard>, 4> directionEdges = { {
			{NORTH, RANK_MASK_8},
			{SOUTH, RANK_MASK_1},
			{EAST, FILE_MASK_H},
			{WEST, FILE_MASK_A}
		} };

		// For each direction
		for (const auto& [d, edge] : directionEdges) {
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				if (edge & next)
					break; // Stop at board edge
				mask |= next;
				temp = static_cast<Square>(temp + d);
			}
		}
		assert(!(mask & squareToBB(sq)));
		return mask;
	}

	// Generate bishop attacks for a given square and occupancy
	Bitboard generateBishopAttacks(const Square sq,const Bitboard occupied)
	{
		assert(isSquare(sq));
		Bitboard attacks = 0;
		// For each diagonal direction
		for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				attacks |= next;
				if (occupied & next)
					break; // Stop at blocking piece
				temp = static_cast<Square>(temp + d);
			}
		}
		assert(!(attacks & squareToBB(sq)));
		return attacks;
	}

	// Generate rook attacks for a given square and occupancy
	Bitboard generateRookAttacks(const Square sq, const Bitboard occupied) {
		assert(isSquare(sq));
		Bitboard attacks = 0;
		// For each direction
		for (const Direction d : {NORTH, SOUTH, EAST, WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				attacks |= next;
				if (occupied & next)
					break; // Stop at blocking piece
				temp = static_cast<Square>(temp + d);
			}
		}
		assert(!(attacks & squareToBB(sq)));
		return attacks;
	}

	// Generate occupancy variation based on index for magic initialization
	Bitboard setOccupancy(const int index,const int bitsInMask, Bitboard mask) {
		assert(index >= 0 && "Index must be non-negative");
		assert(index < (1 << bitsInMask) && "Index exceeds possible combinations");
		assert(bitsInMask >= 0 && bitsInMask <= 64 && "Bits in mask must be between 0 and 64");
		Bitboard occupancy = 0;
		for (int count = 0; count < bitsInMask; count++) {
			const Square sq = popLsb(mask);
			if (index & (1 << count)) {
				occupancy |= squareToBB(sq);
			}
		}
		return occupancy;
	}

	// Find a magic number for the given square and piece type
	MagicResult findMagic(Square square, PieceType pieceType, int bits) {
		assert(isSquare(square) && "Invalid square");
		assert(pieceType >= PieceType::PAWN && pieceType <= PieceType::KING && "Invalid piece type");

		// Get the appropriate mask based on piece type
		const Bitboard mask = (pieceType == BISHOP) ? generateBishopMask(square) : generateRookMask(square);
		const int maskBits = popCount(mask);
		assert(maskBits > 0 && "Mask must have at least one bit set");
		const int occupancyCount = 1 <<maskBits;

		// Create arrays to store all possible occupancies and their corresponding attacks
		std::vector<Bitboard> occupancies(occupancyCount);
		std::vector<Bitboard> attacks(occupancyCount);

		// Generate all possible occupancy variations and their attacks
		for (int i= 0;i<occupancyCount;++i ){
			occupancies.at(i) = setOccupancy(i,maskBits,mask);
			attacks.at(i) = (pieceType == BISHOP) ?
				generateBishopAttacks(square, occupancies.at(i))
				: generateRookAttacks(square, occupancies.at(i));
		}

		// Random number generation for finding magic numbers
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<Bitboard> dist;

		// Try to find a magic number (may require many attempts)
		for (int attempt = 0; attempt < 10000000; attempt++) {
			// Generate a candidate magic number with better bit patterns
			const Bitboard magic = dist(gen) & dist(gen) & dist(gen);
			// Check if magic has good properties
			if (popCount((mask * magic) >> 56) < 6) continue;
			// Test this magic number for collisions
			std::vector<Bitboard> usedAttacks(1ULL << bits, 0);
			std::vector<bool> used(1ULL << bits, false);

			bool failed = false;
			// Check all occupancy variations
			for (int i = 0; i < occupancyCount && !failed; i++) {
				// Calculate index using the magic number
				const Bitboard index = (occupancies.at(i) * magic) >> (64 - bits);

				if (!used.at(index)) {
					// New index - store the attacks
					used.at(index) = true;
					usedAttacks.at(index) = attacks.at(i);
				}
				else if (usedAttacks.at(index) != attacks.at(i)) {
					// Collision with different attack pattern - magic fails
					failed = true;
				}
			}
			// If no collisions, we found a valid magic
			if (!failed) {
				assert(magic);
				return { magic, true };
			}
		}
		// Failed to find a magic
		return { 0, false };
	}
}