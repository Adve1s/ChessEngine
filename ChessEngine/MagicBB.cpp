#include "MagicBB.h"
#include <random>
#include <stdexcept>
#include <vector>

namespace chess {
	Bitboard generateBishopMask(const Square sq)
	{
		Bitboard mask = 0;
		Bitboard edges = FILE_MASK_A | FILE_MASK_H | RANK_MASK_1 | RANK_MASK_8;
		for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				if (edges & next)
					break;
				mask |= next;
				temp = Square(temp + d);
			}
		}
		return mask;
	}

	Bitboard generateRookMask(const Square sq) {
		Bitboard mask = 0;
		constexpr std::pair<Direction, Bitboard> directionEdges[] = {
			{NORTH, RANK_MASK_8},
			{SOUTH, RANK_MASK_1},
			{EAST, FILE_MASK_H},
			{WEST, FILE_MASK_A}
		};

		for (const auto& [d, edge] : directionEdges) {
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				if (edge & next)
					break;
				mask |= next;
				temp = Square(temp + d);
			}
		}
		return mask;
	}

	Bitboard generateBishopAttacks(const Square sq,const Bitboard occupied) {
		Bitboard attacks = 0;
		for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				attacks |= next;
				if (occupied & next)
					break;
				temp = Square(temp + d);
			}
		}
		return attacks;
	}

	Bitboard generateRookAttacks(const Square sq, const Bitboard occupied) {
		Bitboard attacks = 0;
		for (const Direction d : {NORTH, SOUTH, EAST, WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				attacks |= next;
				if (occupied & next)
					break;
				temp = Square(temp + d);
			}
		}
		return attacks;
	}

	Bitboard setOccupancy(const int index,const int bitsInMask, Bitboard mask) {
		Bitboard occupancy = 0;
		for (int count = 0; count < bitsInMask; count++) {
			const Square sq = popLsb(mask);
			if (index & (1 << count)) {
				occupancy |= squareToBB(sq);
			}
		}
		return occupancy;
	}

	Bitboard findMagic(Square square, PieceType pieceType, int bits) {
		Bitboard mask = (pieceType == BISHOP) ? generateBishopMask(square) : generateRookMask(square);
		int maskBits = popCount(mask);
		int occupancyCount = 1 <<maskBits;

		std::vector<Bitboard> occupancies(occupancyCount);
		std::vector<Bitboard> attacks(occupancyCount);

		for (int i= 0;i<occupancyCount;++i ){
			occupancies[i] = setOccupancy(i,maskBits,mask);
			attacks[i] = (pieceType == BISHOP) ?
				generateBishopAttacks(square, occupancies[i])
				: generateRookAttacks(square, occupancies[i]);
		}



		// Random number generation setup
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<Bitboard> dist;

		// Increase attempts significantly
		for (int attempt = 0; attempt < 10000000; attempt++) {
			// Generate a candidate magic number with better properties
			// Using multiple random numbers AND-ed gives sparser bit patterns
			Bitboard magic = dist(gen) & dist(gen) & dist(gen);

			// Ensure high bits have enough randomness (important for the shift operation)
			if (popCount((mask * magic) >> 56) < 6) continue;

			// Test this magic number
			std::vector<Bitboard> usedAttacks(1 << bits, 0);
			std::vector<bool> used(1 << bits, false);
			bool failed = false;

			for (int i = 0; i < occupancyCount && !failed; i++) {
				// Calculate index using the magic number
				Bitboard index = (occupancies[i] * magic) >> (64 - bits);

				if (!used[index]) {
					// New index
					used[index] = true;
					usedAttacks[index] = attacks[i];
				}
				else if (usedAttacks[index] != attacks[i]) {
					// Collision with different attack pattern
					failed = true;
				}
			}

			if (!failed) {
				return magic;
			}
		}

		// Failed to find a magic
		throw std::runtime_error("Failed to find magic number");
	}
}