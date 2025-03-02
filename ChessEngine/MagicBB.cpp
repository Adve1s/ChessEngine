#include "MagicBB.h"
#include <random>
#include <vector>
#include <array>
#include "BitBoard.h"

// MagicBB.cpp - Implementation of magic bitboard functionality

namespace chess {

	// Global arrays for magic bitboards
	std::array<Magic, SQUARE_NB> g_rookMagics;     // Rook magic data for each square
	std::array<Magic, SQUARE_NB> g_bishopMagics;   // Bishop magic data for each square
	std::array<Bitboard, 0x19000> g_rookTable;     // Rook attacks lookup table
	std::array<Bitboard, 0x1480> g_bishopTable;    // Bishop attacks lookup table

	// Generated Bishop Magic Numbers
	const std::array<Bitboard, SQUARE_NB> BISHOP_MAGICS = {
		0xA040381204242020ULL,    0x2008820850410204ULL,    0x20502080830084C6ULL,    0x4019040900015600ULL,    0x5102021048060110ULL,    0xA0A0220000010ULL,    0xC21009005600C00ULL,    0x2000140104222002ULL,
		0x100060808280083ULL,    0x200420820440ULL,    0x9001040840810000ULL,    0x6041052020000ULL,    0x4008020210080080ULL,    0x438060904A00401ULL,    0x950108210036000ULL,    0x810410042422011ULL,
		0x810C083240400ULL,    0x10080802280040ULL,    0x8064018204040008ULL,    0x18000222044001ULL,    0x1810101202100060ULL,    0x800D018CC000ULL,    0x822000082104200ULL,    0x8000800044008804ULL,
		0x80444610C0080802ULL,    0x11042081002A200ULL,    0x8008900008002020ULL,    0x2008008008002ULL,    0x654040008410050ULL,    0x804054088081208ULL,    0x202220002880908ULL,    0x810100C2104100ULL,
		0x101C04C010220200ULL,    0x1015040681001ULL,    0x4004004100880208ULL,    0x288840400080211ULL,    0x40002020120080ULL,    0x310008601602200ULL,    0x241021680040420ULL,    0x20880820A010100ULL,
		0x428582230000801ULL,    0x600441008810428ULL,    0x801209348101001ULL,    0x2018000101ULL,    0x820812012020300ULL,    0x11601084800101ULL,    0x8804040822018040ULL,    0x608080130208040ULL,
		0x4054220104200060ULL,    0x4002108409480000ULL,    0xCA0810041701084ULL,    0x2008000020880000ULL,    0x84008850020220C9ULL,    0x802120270010000ULL,    0x90C1981840808008ULL,    0x8020129180610AD4ULL,
		0x20404048084000ULL,    0x240410082100240ULL,    0x4040205008824ULL,    0x46000000208802ULL,    0x520080040905100ULL,    0x1404A0042908ULL,    0x844590040100ULL,    0x20541102040010ULL
	};

	// Generated Rook Magic Numbers
	const std::array<Bitboard, SQUARE_NB> ROOK_MAGICS = {
		0x2280004002201083ULL,	  0x1280102000400880ULL,    0x1200200842008010ULL,    0x200042042001008ULL,    0x1200080410200201ULL,    0x100080100020400ULL,    0x200010400820008ULL,    0x18014210004C080ULL,
		0xC408800040008020ULL,    0x11402010004002ULL,    0x710801008802000ULL,    0x4000800800801000ULL,    0x100800800800400ULL,    0x1012000804020011ULL,    0xE002002842002104ULL,    0x1C0800080007100ULL,
		0x488002C00040E004ULL,    0x40002010002801ULL,    0x10002020040800ULL,    0x86120008402200ULL,    0x820050008010090ULL,    0x808002000400ULL,    0x4308040001221088ULL,    0x4020001008074ULL,
		0x20800080204000ULL,    0x200040100040ULL,    0x42E0010100261040ULL,    0x2548000880801000ULL,    0xE000040080800800ULL,    0x8188040080020080ULL,    0x180D00400088241ULL,    0x10406200008401ULL,
		0x480002004400040ULL,    0x8821008042002208ULL,    0x1240801000802002ULL,    0x5041001000820ULL,    0x50080080C8804400ULL,    0x800800200800400ULL,    0xE22000100408040ULL,    0x802000B102000044ULL,
		0x980804000308000ULL,    0x2000200040008080ULL,    0x1A12002044820010ULL,    0x204200120009ULL,    0xC000800808004ULL,    0x2000408020010ULL,    0x14025001040008ULL,    0x9040010040A20004ULL,
		0x1004424800500ULL,    0x1220810200205200ULL,    0x500802000100080ULL,    0x30100408210100ULL,    0x8201900800850100ULL,    0x40E0044010200801ULL,    0x401001426000900ULL,    0x2421000082004100ULL,
		0x81810013260042ULL,    0x88522801A004102ULL,    0x108402001100903ULL,    0x4001002190000409ULL,    0x5000410020801ULL,    0xA001001088402ULL,    0x40498100A4104ULL,    0x800040020408102ULL
	};
	namespace magicBB {
		void init()
		{
			initMagics(BISHOP);
			initMagics(ROOK);
		}
	}
	// Initialize magic bitboards for a piece type
	void initMagics(const PieceType piece)
	{
		assert(piece == ROOK || piece == BISHOP);
		assert(popCount(squareToBB(A2)) == 1);
		assert(setOccupancy(1, 1, squareToBB(E4)) == squareToBB(E4));
		// Clear the attack table before filling
		if (piece == BISHOP) std::ranges::fill(g_bishopTable, 0);
		else std::ranges::fill(g_rookTable, 0);
		int offSet = 0;
		for (Square sq = A1; sq < SQUARE_NB; ++sq) {
			Magic& currMagic = (piece == BISHOP) ? g_bishopMagics.at(sq) : g_rookMagics.at(sq);
			// Generate mask of relevant squares
			currMagic.mask = (piece == BISHOP) ? generateBishopMask(sq) : generateRookMask(sq);
			assert(currMagic.mask);
			const int bits = popCount(currMagic.mask);
			currMagic.shift = 64 - bits;
			// Use magic numbers, from pre-calculated array, if needed findMagic() can be used to find numbers
			currMagic.magic = (piece == BISHOP) ? BISHOP_MAGICS.at(sq) : ROOK_MAGICS.at(sq);
			const int variations = 1 << bits;
			// Set up span to point to the right segment of the table
			currMagic.attacks = (piece == BISHOP) ? gsl::span(&g_bishopTable.at(offSet), variations) : gsl::span(&g_rookTable.at(offSet), variations);
			offSet += variations;
			// Fill the attack table for all possible occupancy patterns
			for (int index = 0; index < variations; ++index)
			{
				const Bitboard occupancy = setOccupancy(index, bits, currMagic.mask);
				const unsigned int magicIndex = currMagic.getIndex(occupancy);
				const Bitboard attacks = (piece == BISHOP) ? generateBishopAttacks(sq, occupancy) : generateRookAttacks(sq, occupancy);
				assert(attacks);
				currMagic.attacks[magicIndex] = attacks;
			}
		}
	}

	// Generate bishop mask (excludes edges and the source square)
	Bitboard generateBishopMask(const Square sq)
	{
		assert(isSquare(sq));
		assert(insideBoard(A1, 1) && !insideBoard(A1, 27));
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
				temp = temp + d;
			}
		}
		assert(!(mask & squareToBB(sq)));
		return mask;
	}

	// Generate rook mask (excludes edges and the source square)
	Bitboard generateRookMask(const Square sq)
	{
		assert(isSquare(sq));
		assert(insideBoard(A1, 1) && !insideBoard(A1, 27));
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
				temp = temp + d;
			}
		}
		assert(!(mask & squareToBB(sq)));
		return mask;
	}

	// Generate bishop attacks for a given square and occupancy
	Bitboard generateBishopAttacks(const Square sq,const Bitboard occupied)
	{
		assert(isSquare(sq));
		assert(insideBoard(A1, 1) && !insideBoard(A1, 27));
		Bitboard attacks = 0;
		// For each diagonal direction
		for (const Direction d : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				attacks |= next;
				if (occupied & next)
					break; // Stop at blocking piece
				temp = temp + d;
			}
		}
		assert(!(attacks & squareToBB(sq)));
		return attacks;
	}

	// Generate rook attacks for a given square and occupancy
	Bitboard generateRookAttacks(const Square sq, const Bitboard occupied) {
		assert(isSquare(sq));
		assert(insideBoard(A1, 1) && !insideBoard(A1, 27));
		Bitboard attacks = 0;
		// For each direction
		for (const Direction d : {NORTH, SOUTH, EAST, WEST})
		{
			Square temp = sq;
			while (const Bitboard next = insideBoard(temp, d)) {
				attacks |= next;
				if (occupied & next)
					break; // Stop at blocking piece
				temp = temp + d;
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
	MagicResult findMagic(const Square square, const PieceType pieceType) {
		assert(isSquare(square) && "Invalid square");
		assert(pieceType >= PieceType::PAWN && pieceType <= PieceType::KING && "Invalid piece type");
		assert(popCount(squareToBB(A2)) == 1);
		assert(setOccupancy(1, 1, squareToBB(E4)) == squareToBB(E4));

		// Get the appropriate mask based on piece type
		const Bitboard mask = (pieceType == BISHOP) ? generateBishopMask(square) : generateRookMask(square);
		assert(mask);
		const int maskBits = popCount(mask);
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
		assert(attacks.at(0));

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
			std::vector<Bitboard> usedAttacks(1ULL << maskBits, 0);
			std::vector<bool> used(1ULL << maskBits, false);

			bool failed = false;
			// Check all occupancy variations
			for (int i = 0; i < occupancyCount && !failed; i++) {
				// Calculate index using the magic number
				const int shift = 64 - maskBits;
				assert(shift >= 0 && shift < 64 && "Invalid shift amount");
				const Bitboard index = (occupancies.at(i) * magic) >> shift;
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
				return { magic,true };
			}
		}
		// Failed to find a magic
		return { 0, false };
	}

	// Gets bishop attacks for a square using magic bitboards
	Bitboard getBishopAttacks(const Square sq, const Bitboard occupied) {
		assert(isSquare(sq));
		const Magic& magic = g_bishopMagics.at(sq);
		// Get index from magic multiplication
		const unsigned int index = magic.getIndex(occupied & magic.mask);
		return magic.attacks[index];
	}

	Bitboard getRookAttacks(const Square sq, const Bitboard occupied) {
		assert(isSquare(sq));
		const Magic& magic = g_rookMagics.at(sq);
		// Get index from magic multiplication
		const unsigned int index = magic.getIndex(occupied & magic.mask);
		return magic.attacks[index];
	}

	// Gets queen attacks (combination of bishop and rook attacks)
	Bitboard getQueenAttacks(const Square sq, const Bitboard occupied) {
		return getBishopAttacks(sq, occupied) | getRookAttacks(sq, occupied);
	}
}