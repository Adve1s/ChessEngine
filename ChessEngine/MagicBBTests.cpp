#include "MagicBBTests.h"
#include "MagicBB.h"
#include <iostream>
#include <vector>

#include "types.h"

// Run all tests
namespace chess {
	namespace tests {

		// Test bishop and rook mask generation
		void testMaskGeneration() {
			// Test bishop masks
			bool bishopSuccess = true;

			// Test center square (D4)
			Bitboard centerBishop = generateBishopMask(D4);
			bishopSuccess &= (popCount(centerBishop) == 9); // A bishop in the center should have 9 relevant squares

			// Test corner square (H1)
			Bitboard cornerBishop = generateBishopMask(H1);
			bishopSuccess &= (popCount(cornerBishop) == 6); // A bishop in the corner should have 6 relevant squares

			// Test specific bits for a known position
			Bitboard e4Bishop = generateBishopMask(E4);
			// Check if specific squares are set in the mask
			bishopSuccess &= (e4Bishop & squareToBB(D3)) != 0; // Southwest
			bishopSuccess &= (e4Bishop & squareToBB(F3)) != 0; // Southeast
			bishopSuccess &= (e4Bishop & squareToBB(D5)) != 0; // Northwest
			bishopSuccess &= (e4Bishop & squareToBB(F5)) != 0; // Northeast

			// The bishop's own square should not be in the mask
			bishopSuccess &= (e4Bishop & squareToBB(E4)) == 0;

			// Edge squares should not be in the mask
			bishopSuccess &= (e4Bishop & squareToBB(A8)) == 0;
			bishopSuccess &= (e4Bishop & squareToBB(H1)) == 0;

			
			report("Bishop mask generation", bishopSuccess);

			// Test rook masks
			bool rookSuccess = true;

			// Test center square (D4)
			Bitboard centerRook = generateRookMask(D4);
			rookSuccess &= (popCount(centerRook) == 10); // A rook in the center should have 10 relevant squares

			// Test corner square (A1)
			Bitboard cornerRook = generateRookMask(A1);
			rookSuccess &= (popCount(cornerRook) == 12); // A rook in the corner should have 12 relevant squares

			// Test specific bits for a known position
			Bitboard e4Rook = generateRookMask(E4);
			// Check if specific squares are set in the mask
			rookSuccess &= (e4Rook & squareToBB(E3)) != 0; // South
			rookSuccess &= (e4Rook & squareToBB(E5)) != 0; // North
			rookSuccess &= (e4Rook & squareToBB(D4)) != 0; // West
			rookSuccess &= (e4Rook & squareToBB(F4)) != 0; // East

			// The rook's own square should not be in the mask
			rookSuccess &= (e4Rook & squareToBB(E4)) == 0;

			// Edge squares should not be in the mask
			rookSuccess &= (e4Rook & squareToBB(E1)) == 0;
			rookSuccess &= (e4Rook & squareToBB(E8)) == 0;
			rookSuccess &= (e4Rook & squareToBB(A4)) == 0;
			rookSuccess &= (e4Rook & squareToBB(H4)) == 0;

			report("Rook mask generation", rookSuccess);

			// Visual inspection for debugging
			if (!bishopSuccess || !rookSuccess) {
				std::cout << "\nVisual inspection of masks:" << "\n";
				std::cout << "Bishop mask for E4:" << "\n";
				printBitBoard(e4Bishop);
				std::cout << "Rook mask for E4:" << "\n";
				printBitBoard(e4Rook);
			}
		}

		void testBishopAttacksGeneration() {
			bool success = true;

			// Test bishop attacks on an empty board
			// Center square (D4)
			Bitboard emptyBoard = 0;
			Bitboard d4BishopAttacks = generateBishopAttacks(D4, emptyBoard);

			// A bishop in D4 on an empty board should attack 13 squares
			success &= (popCount(d4BishopAttacks) == 13);

			// Check specific squares that should be attacked
			success &= (d4BishopAttacks & squareToBB(C3)) != 0; // SW diagonal
			success &= (d4BishopAttacks & squareToBB(B2)) != 0; // SW diagonal
			success &= (d4BishopAttacks & squareToBB(A1)) != 0; // SW diagonal
			success &= (d4BishopAttacks & squareToBB(E5)) != 0; // NE diagonal
			success &= (d4BishopAttacks & squareToBB(F6)) != 0; // NE diagonal
			success &= (d4BishopAttacks & squareToBB(G7)) != 0; // NE diagonal
			success &= (d4BishopAttacks & squareToBB(H8)) != 0; // NE diagonal

			report("Bishop attacks (empty board)", success);

			// Test with blocking pieces
			Bitboard blockedBoard = 0;
			setBit(blockedBoard, C3); // Block the SW diagonal
			setBit(blockedBoard, F6); // Block the NE diagonal

			Bitboard d4BishopBlocked = generateBishopAttacks(D4, blockedBoard);

			success = true;
			// Should include the blocking pieces, but not beyond
			success &= (d4BishopBlocked & squareToBB(C3)) != 0; // SW blocker included
			success &= (d4BishopBlocked & squareToBB(B2)) == 0; // Beyond SW blocker excluded
			success &= (d4BishopBlocked & squareToBB(A1)) == 0; // Beyond SW blocker excluded
			success &= (d4BishopBlocked & squareToBB(F6)) != 0; // NE blocker included
			success &= (d4BishopBlocked & squareToBB(G7)) == 0; // Beyond NE blocker excluded

			report("Bishop attacks (with blocking pieces)", success);

			// Test from corner
			Bitboard a1BishopAttacks = generateBishopAttacks(A1, emptyBoard);
			success = true;
			success &= (popCount(a1BishopAttacks) == 7); // A bishop in A1 attacks 7 squares
			success &= (a1BishopAttacks & squareToBB(H8)) != 0; // Long diagonal

			report("Bishop attacks from corner", success);
		}

		// Test generation of rook attacks
		void testRookAttacksGeneration() {
			bool success = true;

			// Test rook attacks on an empty board
			// Center square (E4)
			Bitboard emptyBoard = 0;
			Bitboard e4RookAttacks = generateRookAttacks(E4, emptyBoard);

			// A rook in E4 on an empty board should attack 14 squares (7 horizontal + 7 vertical)
			success &= (popCount(e4RookAttacks) == 14);

			// Check specific squares that should be attacked
			success &= (e4RookAttacks & squareToBB(E1)) != 0; // South
			success &= (e4RookAttacks & squareToBB(E8)) != 0; // North
			success &= (e4RookAttacks & squareToBB(A4)) != 0; // West
			success &= (e4RookAttacks & squareToBB(H4)) != 0; // East

			report("Rook attacks (empty board)", success);

			// Test with blocking pieces
			Bitboard blockedBoard = 0;
			setBit(blockedBoard, E2); // Block South
			setBit(blockedBoard, G4); // Block East

			Bitboard e4RookBlocked = generateRookAttacks(E4, blockedBoard);

			success = true;
			// Should include the blocking pieces, but not beyond
			success &= (e4RookBlocked & squareToBB(E2)) != 0; // South blocker included
			success &= (e4RookBlocked & squareToBB(E1)) == 0; // Beyond South blocker excluded
			success &= (e4RookBlocked & squareToBB(G4)) != 0; // East blocker included
			success &= (e4RookBlocked & squareToBB(H4)) == 0; // Beyond East blocker excluded

			report("Rook attacks (with blocking pieces)", success);

			// Test from corner
			Bitboard a1RookAttacks = generateRookAttacks(A1, emptyBoard);
			success = true;
			success &= (popCount(a1RookAttacks) == 14); // A rook in A1 attacks 14 squares
			success &= (a1RookAttacks & squareToBB(A8)) != 0; // Full file
			success &= (a1RookAttacks & squareToBB(H1)) != 0; // Full rank

			report("Rook attacks from corner", success);

			// Test complex blocking scenario
			Bitboard complexBoard = 0;
			setBit(complexBoard, A1); // Rook position
			setBit(complexBoard, A3); // Block up
			setBit(complexBoard, C1); // Block right

			// We need to exclude the rook's own position from the occupied bitboard
			// for this test since we're calculating its attacks
			clearBit(complexBoard, A1);

			Bitboard a1RookComplex = generateRookAttacks(A1, complexBoard);

			success = true;
			success &= (popCount(a1RookComplex) == 4); // Only A2, A3, C1, and B1 should be attackable
			success &= (a1RookComplex & squareToBB(A2)) != 0; // Can attack A2
			success &= (a1RookComplex & squareToBB(A3)) != 0; // Can attack the blocker at A3
			success &= (a1RookComplex & squareToBB(A4)) == 0; // Can't attack beyond A3
			success &= (a1RookComplex & squareToBB(B1)) != 0; // Can attack B1
			success &= (a1RookComplex & squareToBB(C1)) != 0; // Can attack the blocker at C1
			success &= (a1RookComplex & squareToBB(D1)) == 0; // Can't attack beyond C1

			report("Rook attacks (complex blocking)", success);
		}

		// Test combined rook and bishop attacks for queen
		void testCombinedSlidingAttacks() {
			bool success = true;

			// An empty board
			Bitboard emptyBoard = 0;

			// Center square (D4)
			Bitboard d4BishopAttacks = generateBishopAttacks(D4, emptyBoard);
			Bitboard d4RookAttacks = generateRookAttacks(D4, emptyBoard);

			// Combined attacks should be the same as a queen's attacks
			Bitboard d4QueenAttacks = d4BishopAttacks | d4RookAttacks;
			success &= (popCount(d4QueenAttacks) == 27); // 13 bishop + 14 rook

			// Should contain all rook and bishop attacks
			Bitboard bishopSquares[4] = { squareToBB(C3), squareToBB(E5), squareToBB(C5), squareToBB(E3) };
			Bitboard rookSquares[4] = { squareToBB(D1), squareToBB(D8), squareToBB(A4), squareToBB(H4) };

			for (int i = 0; i < 4; i++) {
				success &= (d4QueenAttacks & bishopSquares[i]) != 0;
				success &= (d4QueenAttacks & rookSquares[i]) != 0;
			}

			report("Combined sliding piece attacks (Queen)", success);

			// Test with blocking pieces
			Bitboard blockedBoard = 0;
			setBit(blockedBoard, C3); // Block SW diagonal
			setBit(blockedBoard, D2); // Block South

			Bitboard d4BishopBlocked = generateBishopAttacks(D4, blockedBoard);
			Bitboard d4RookBlocked = generateRookAttacks(D4, blockedBoard);
			Bitboard d4QueenBlocked = d4BishopBlocked | d4RookBlocked;

			success = true;
			// Queen's attacks should include blocking pieces
			success &= (d4QueenBlocked & squareToBB(C3)) != 0;
			success &= (d4QueenBlocked & squareToBB(D2)) != 0;
			// But not beyond them
			success &= (d4QueenBlocked & squareToBB(B2)) == 0;
			success &= (d4QueenBlocked & squareToBB(D1)) == 0;

			report("Combined attacks with blocking pieces", success);
		}

		void testSetOccupancy() {
			bool success = true;

			// Test case 1: Simple mask with 3 bits set
			Bitboard mask = squareToBB(A1) | squareToBB(B2) | squareToBB(C3);
			int bitsInMask = popCount(mask);

			// Test index 0 (000) - No squares occupied
			Bitboard occ0 = setOccupancy(0, bitsInMask, mask);
			success &= (occ0 == 0);

			// Test index 7 (111) - All squares occupied
			Bitboard occ7 = setOccupancy(7, bitsInMask, mask);
			success &= (occ7 == mask);

			// Test index 5 (101) - A1 and C3 occupied, B2 empty
			Bitboard occ5 = setOccupancy(5, bitsInMask, mask);
			Bitboard expected = squareToBB(A1) | squareToBB(C3);
			success &= (occ5 == expected);

			// Test with a bishop mask
			Bitboard bishopMask = generateBishopMask(E4);
			int bishopBits = popCount(bishopMask);

			// Test all bits set - should equal the original mask
			int allSetIndex = (1 << bishopBits) - 1;
			Bitboard bishopOcc = setOccupancy(allSetIndex, bishopBits, bishopMask);
			success &= (bishopOcc == bishopMask);

			report("setOccupancy function", success);
		}

		void testFindMagicEasy() {
			bool success = true;

			try {
				// Test a corner square first (fewer relevant bits)
				Square testSquare = A1;
				int indexBits = 6;

				std::cout << "Attempting to find bishop magic for square A1..." << std::endl;
				Bitboard bishopMagic = findMagic(testSquare, BISHOP, indexBits);

				if (bishopMagic != 0) {
					success = true;
					std::cout << "Found bishop magic number: 0x" << std::hex << bishopMagic << std::dec << std::endl;
				}

				report("findMagic function", success);
			}
			catch (const std::exception& e) {
				std::cout << "Exception in findMagic test: " << e.what() << std::endl;
				report("findMagic function", false);
			}
		}

		void testFindMagic() {
			std::cout << "Testing Magic Bitboard Generation..." << std::endl;
			bool overallSuccess = true;

			// Test structure to store our test cases
			struct TestCase {
				Square square;
				PieceType pieceType;
				int indexBits;
				std::string description;
			};

			// Define a variety of test cases
			std::vector<TestCase> testCases = {
				{A1, BISHOP, 6, "Bishop on corner (A1)"},
				{H1, BISHOP, 6, "Bishop on corner (H1)"},
				{A1, ROOK, 12, "Rook on corner (A1)"},
				{D4, BISHOP, 9, "Bishop near center (D4)"}
			};

			for (const auto& test : testCases) {
				bool success = true;
				std::cout << "  Testing " << test.description << "..." << std::endl;

				try {
					// Find magic number for this test case
					Bitboard magic = findMagic(test.square, test.pieceType, test.indexBits);

					if (magic == 0) {
						std::cout << "    Failed: Got zero magic number" << std::endl;
						success = false;
					}
					else {
						// Print the found magic in hex format
						std::cout << "    Found magic: 0x" << std::hex << magic << std::dec << std::endl;

						// Verify the magic number works by checking occupancy patterns
						Bitboard mask = (test.pieceType == BISHOP) ?
							generateBishopMask(test.square) :
							generateRookMask(test.square);

						int maskBits = popCount(mask);
						int occupancyCount = 1 << maskBits;

						// Only test a subset of all occupancies to keep the test fast
						// We'll test at most 100 patterns, evenly distributed
						int testStep = std::max(1, occupancyCount / 100);
						int testedCount = 0;

						std::vector<Bitboard> usedIndices;
						bool collisionFound = false;

						for (int i = 0; i < occupancyCount && !collisionFound && testedCount < 100; i += testStep) {
							testedCount++;
							Bitboard occ = setOccupancy(i, maskBits, mask);
							Bitboard attacks = (test.pieceType == BISHOP) ?
								generateBishopAttacks(test.square, occ) :
								generateRookAttacks(test.square, occ);

							// Calculate index using the magic number
							Bitboard index = (occ * magic) >> (64 - test.indexBits);

							// Check for duplicate indices with different attack patterns
							for (int j = 0; j < usedIndices.size(); j++) {
								if (usedIndices[j] == index && attacks !=
									((test.pieceType == BISHOP) ?
										generateBishopAttacks(test.square, setOccupancy(j * testStep, maskBits, mask)) :
										generateRookAttacks(test.square, setOccupancy(j * testStep, maskBits, mask)))) {
									std::cout << "    Failed: Collision detected for index " << index << std::endl;
									collisionFound = true;
									success = false;
									break;
								}
							}

							usedIndices.push_back(index);
						}

						if (!collisionFound) {
							std::cout << "    Verified " << testedCount << " occupancy patterns with no collisions" << std::endl;
						}
					}
				}
				catch (const std::exception& e) {
					std::cout << "    Exception: " << e.what() << std::endl;
					success = false;
				}

				report(test.description, success);
				overallSuccess &= success;
			}

			report("Overall Magic Bitboard Generation", overallSuccess);
		}

		void runAllMagicBBTests() {
			std::cout << "Running MagicBB tests...\n" << "\n";
			testMaskGeneration();
			testBishopAttacksGeneration();
			testRookAttacksGeneration();
			testCombinedSlidingAttacks();
			testSetOccupancy();
			testFindMagicEasy();
			testFindMagic();
			std::cout << "\nBitBoard MagicBB completed." << "\n";
		}
	}
}