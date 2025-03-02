#include "BitBoardTests.h"
#include <iostream>
#include "BitBoard.h"

namespace chess::tests
{
	// Test basic bitboard operations
	void testBasicOperations() {
		// Test squareToBB
		bool success = true;

		// Test for a few representative squares
		success &= (squareToBB(A1) == 0x1ULL);
		success &= (squareToBB(H1) == 0x80ULL);
		success &= (squareToBB(A8) == 0x100000000000000ULL);
		success &= (squareToBB(H8) == 0x8000000000000000ULL);
		success &= (squareToBB(E4) == 0x10000000ULL);

		report("Basic squareToBB", success);

		// Test setBit/clearBit
		success = true;
		Bitboard b = 0;
		setBit(b, E4);
		success &= (b == squareToBB(E4));
		setBit(b, F5);
		success &= (b == (squareToBB(E4) | squareToBB(F5)));
		clearBit(b, E4);
		success &= (b == squareToBB(F5));
		clearBit(b, F5);
		success &= (b == Bitboard{ 0 });

		report("setBit/clearBit", success);

		// Test isSquareEmpty
		success = true;
		constexpr Bitboard board = squareToBB(A1) | squareToBB(H8);
		success &= !isSquareEmpty(board, A1);
		success &= !isSquareEmpty(board, H8);
		success &= isSquareEmpty(board, E4);

		report("isSquareEmpty", success);
	}

	// Test bit manipulation functions
	void testBitManipulation() {
		// Test lsb
		Bitboard b = squareToBB(C3) | squareToBB(F7);
		bool success = (lsb(b) == C3);
		report("lsb function", success);

		// Test msb
		success = (msb(b) == F7);
		report("msb function", success);

		// Test pop_lsb
		b = squareToBB(C3) | squareToBB(F7);
		Square s = popLsb(b);
		success = (s == C3) && (b == squareToBB(F7));
		s = popLsb(b);
		success &= (s == F7) && (b == Bitboard(0));
		report("pop_lsb function", success);

		// Test popCount
		b = squareToBB(A1) | squareToBB(B2) | squareToBB(C3);
		success = (popCount(b) == 3);
		b = 0;
		success &= (popCount(b) == 0);
		report("popCount function", success);
	}

	// Test distance calculations
	void testDistanceFunctions() {
		// Initialize the distance tables
		bitboards::init();

		bool success = true;

		// Test file distance
		success &= (distance<File>(A1, A2) == 0);
		success &= (distance<File>(A1, C5) == 2);
		success &= (distance<File>(H8, A8) == 7);

		report("File distance", success);

		// Test rank distance
		success = true;
		success &= (distance<Rank>(A1, B1) == 0);
		success &= (distance<Rank>(A1, A3) == 2);
		success &= (distance<Rank>(H1, H8) == 7);

		report("Rank distance", success);

		// Test square distance (which uses the maximum of file and rank distances)
		success = true;
		success &= (distance<Square>(A1, A1) == 0);
		success &= (distance<Square>(A1, B2) == 1);
		success &= (distance<Square>(A1, H8) == 7);
		success &= (distance<Square>(E4, A8) == 4);

		report("Square distance", success);
	}

	// Test pawn attack patterns
	void testPawnAttacks() {
		bitboards::init(); // Ensure attacks are initialized

		bool success = true;

		// Test white pawn attacks
		success &= (g_pawnAttacks.at(WHITE).at(A2) == (squareToBB(B3)));
		success &= (g_pawnAttacks.at(WHITE).at(E4) == (squareToBB(D5) | squareToBB(F5)));
		success &= (g_pawnAttacks.at(WHITE).at(H7) == (squareToBB(G8)));
		success &= (g_pawnAttacks.at(WHITE).at(A8) == Bitboard{ 0 });

		report("White pawn attacks", success);

		// Test black pawn attacks
		success = true;
		success &= (g_pawnAttacks.at(BLACK).at(A7) == (squareToBB(B6)));
		success &= (g_pawnAttacks.at(BLACK).at(E5) == (squareToBB(D4) | squareToBB(F4)));
		success &= (g_pawnAttacks.at(BLACK).at(H2) == (squareToBB(G1)));
		success &= (g_pawnAttacks.at(BLACK).at(A1) == Bitboard{ 0 });

		report("Black pawn attacks", success);
	}

	// Test knight attack patterns
	void testKnightAttacks() {
		bitboards::init();

		bool success = true;

		// Middle of board
		const Bitboard e4Knights = g_pseudoAttacks.at(KNIGHT).at(E4);
		success &= (popCount(e4Knights) == 8);
		success &= (e4Knights & squareToBB(D6)) != 0;
		success &= (e4Knights & squareToBB(F6)) != 0;
		success &= (e4Knights & squareToBB(G5)) != 0;
		success &= (e4Knights & squareToBB(G3)) != 0;
		success &= (e4Knights & squareToBB(F2)) != 0;
		success &= (e4Knights & squareToBB(D2)) != 0;
		success &= (e4Knights & squareToBB(C3)) != 0;
		success &= (e4Knights & squareToBB(C5)) != 0;

		report("Knight attacks (middle)", success);

		// Corner of board
		const Bitboard a1Knights = g_pseudoAttacks.at(KNIGHT).at(A1);
		success = true;
		success &= (popCount(a1Knights) == 2);
		success &= (a1Knights & squareToBB(B3)) != 0;
		success &= (a1Knights & squareToBB(C2)) != 0;

		report("Knight attacks (corner)", success);
	}

	// Test king attack patterns
	void testKingAttacks() {
		bitboards::init();

		bool success = true;

		// Middle of board
		const Bitboard e4King = g_pseudoAttacks.at(KING).at(E4);
		success &= (popCount(e4King) == 8);

		// Check each square the king can move to
		success &= (e4King & squareToBB(D3)) != 0;
		success &= (e4King & squareToBB(D4)) != 0;
		success &= (e4King & squareToBB(D5)) != 0;
		success &= (e4King & squareToBB(E3)) != 0;
		success &= (e4King & squareToBB(E5)) != 0;
		success &= (e4King & squareToBB(F3)) != 0;
		success &= (e4King & squareToBB(F4)) != 0;
		success &= (e4King & squareToBB(F5)) != 0;

		report("King attacks (middle)", success);

		// Corner of board
		const Bitboard a1King = g_pseudoAttacks.at(KING).at(A1);
		success = true;
		success &= (popCount(a1King) == 3);
		success &= (a1King & squareToBB(A2)) != 0;
		success &= (a1King & squareToBB(B1)) != 0;
		success &= (a1King & squareToBB(B2)) != 0;

		report("King attacks (corner)", success);
	}

	// Test sliding piece attack patterns
	void testSlidingPieceAttacks() {
		bitboards::init();

		// Test rook attacks
		bool success = true;
		const Bitboard e4Rook = g_pseudoAttacks.at(ROOK).at(E4);

		// Check count - should be 14 squares (7 horizontal + 7 vertical, excluding E4)
		success &= (popCount(e4Rook) == 14);

		// Check ranks and files
		constexpr Bitboard e4Rank = RANK_MASK_4 & ~squareToBB(E4);
		constexpr Bitboard e4File = FILE_MASK_E & ~squareToBB(E4);
		success &= ((e4Rook & e4Rank) == e4Rank);
		success &= ((e4Rook & e4File) == e4File);

		report("Rook attacks", success);

		// Test bishop attacks
		success = true;
		const Bitboard e4Bishop = g_pseudoAttacks.at(BISHOP).at(E4);

		// There should be 13 squares a bishop can attack from E4
		// (NE: 3, SE: 3, SW: 3, NW: 4)
		success &= (popCount(e4Bishop) == 13);

		// Test some specific diagonal squares
		success &= (e4Bishop & squareToBB(B1)) != 0;  // SW
		success &= (e4Bishop & squareToBB(H7)) != 0;  // NE
		success &= (e4Bishop & squareToBB(B7)) != 0;  // NW
		success &= (e4Bishop & squareToBB(H1)) != 0;  // SE

		report("Bishop attacks", success);

		// Test queen attacks (combination of rook and bishop)
		success = true;
		const Bitboard e4Queen = g_pseudoAttacks.at(QUEEN).at(E4);
		success &= (e4Queen == (e4Rook | e4Bishop));
		success &= (popCount(e4Queen) == 27); // 14 rook + 13 bishop

		report("Queen attacks", success);
	}

	// Test between/through squares
	void testBetweenAndThroughSquares() {
		bitboards::init();

		bool success = true;

		// Test between squares
		// Between A1 and A5 should be A2, A3, A4
		Bitboard between = g_betweenBB.at(A1).at(A5);
		success &= (popCount(between) == 3);
		success &= (between & squareToBB(A2)) != 0;
		success &= (between & squareToBB(A3)) != 0;
		success &= (between & squareToBB(A4)) != 0;

		// Diagonal between A1 and D4 should be B2, C3
		between = g_betweenBB.at(A1).at(D4);
		success &= (popCount(between) == 2);
		success &= (between & squareToBB(B2)) != 0;
		success &= (between & squareToBB(C3)) != 0;

		// No diagonal between A1 and C8, should return C8
		between = g_betweenBB.at(A1).at(C8);
		success &= (popCount(between) == 1);
		success &= (between & squareToBB(C8)) != 0;

		// In place A1 and A1, should be empty
		between = g_betweenBB.at(A1).at(A1);
		success &= (popCount(between) == 0);
		success &= between == 0;

		report("Between squares", success);

		// Test through squares
		// Through A1 to H8 should include the diagonal and beyond
		Bitboard through = g_throughBB.at(A1).at(H8);
		success = true;

		// Should include all squares on the a1-h8 diagonal
		for (const Square s : {A1, B2, C3, D4, E5, F6, G7, H8}) {
			success &= (through & squareToBB(s)) != 0;
		}

		// H5 and D5, should be A5 to H5
		through = g_throughBB.at(H5).at(D5);
		for (const Square s : {A5, B5, C5, D5, E5, F5, G5, H5}) {
			success &= (through & squareToBB(s)) != 0;
		}

		// A1 and D2, should return empty
		through = g_throughBB.at(A1).at(D2);
		success &= through == 0;

		// A1 and A1, should return A1
		through = g_throughBB.at(A1).at(A1);
		success &= (through & squareToBB(A1)) != 0;

		report("Through squares", success);
	}

	// Test string/square conversion
	void testStringSquareConversion() {
		bool success = true;

		// Convert string to square
		success &= (stringToSquare("a1") == A1);
		success &= (stringToSquare("e4") == E4);
		success &= (stringToSquare("h8") == H8);
		success &= (stringToSquare("-") == NO_SQUARE);
		success &= (stringToSquare("invalid") == NO_SQUARE);

		report("String to square conversion", success);

		// Convert square to string
		success = true;
		success &= (squareToString(A1) == "a1");
		success &= (squareToString(E4) == "e4");
		success &= (squareToString(H8) == "h8");

		report("Square to string conversion", success);
	}

	void testMaskBitboards() {
		// Test file masks
		bool success = true;
		success &= (popCount(FILE_MASK_A) == 8);

		// Test rank masks
		success &= (popCount(RANK_MASK_1) == 8);

		// Test that file and rank intersections give the correct squares
		success &= ((RANK_MASK_1 & FILE_MASK_A) == squareToBB(A1));
		success &= ((RANK_MASK_1 & FILE_MASK_D) == squareToBB(D1));
		success &= ((RANK_MASK_1 & FILE_MASK_H) == squareToBB(H1));

		success &= ((RANK_MASK_4 & FILE_MASK_A) == squareToBB(A4));
		success &= ((RANK_MASK_4 & FILE_MASK_D) == squareToBB(D4));
		success &= ((RANK_MASK_4 & FILE_MASK_H) == squareToBB(H4));

		success &= ((RANK_MASK_8 & FILE_MASK_A) == squareToBB(A8));
		success &= ((RANK_MASK_8 & FILE_MASK_D) == squareToBB(D8));
		success &= ((RANK_MASK_8 & FILE_MASK_H) == squareToBB(H8));

		std::cout << (success ? "PASS: " : "FAIL: ") << "Mask bitboards" << "\n";
	}

	void testInsideBoard() {
		bool success = true;

		// Test moves that stay on board
		success &= (insideBoard(E4, NORTH) == squareToBB(E5));
		success &= (insideBoard(E4, NORTH_EAST) == squareToBB(F5));

		// Test edge of board
		success &= (insideBoard(H4, EAST) == 0);
		success &= (insideBoard(A4, WEST) == 0);
		success &= (insideBoard(E8, NORTH) == 0);
		success &= (insideBoard(E1, SOUTH) == 0);

		// Test corners
		success &= (insideBoard(H8, NORTH) == 0);
		success &= (insideBoard(H8, EAST) == 0);
		success &= (insideBoard(H8, NORTH_EAST) == 0);
		success &= (insideBoard(H8, SOUTH_WEST) == squareToBB(G7));

		// Test knight moves
		success &= (insideBoard(E4, 17) == squareToBB(F6)); // Knight move NNE
		success &= (insideBoard(B2, -10) == 0); // Knight move would go off board

		std::cout << (success ? "PASS: " : "FAIL: ") << "insideBoard function" << "\n";
	}

	// Test direction calculation
	void testDirectionCalculation() {
		bool success = true;

		// Test cardinal directions
		success &= (getDirection(E4, E8) == NORTH);
		success &= (getDirection(E4, E1) == SOUTH);
		success &= (getDirection(E4, H4) == EAST);
		success &= (getDirection(E4, A4) == WEST);

		// Test diagonal directions
		success &= (getDirection(E4, H7) == NORTH_EAST);
		success &= (getDirection(E4, B7) == NORTH_WEST);
		success &= (getDirection(E4, H1) == SOUTH_EAST);
		success &= (getDirection(E4, B1) == SOUTH_WEST);

		// Test invalid directions (when squares aren't aligned)
		success &= (getDirection(E4, F6) == Direction{ 0 });

		report("Direction calculation", success);
	}

	void testPrintBitBoard() {
		// Simple test to ensure function doesn't crash
		constexpr Bitboard b = squareToBB(E4) | squareToBB(F5) | FILE_MASK_A | RANK_MASK_7;
		std::cout << "Testing printBitBoard (visual inspection required, should be E4, F5, A file, 7 Rank):" << "\n";
		printBitBoard(b);
		std::cout << "MANUAL CHECK: printBitBoard function" << "\n";
	}

	// Run all tests
	void runAllBitBoardTests() {
		std::cout << "Running BitBoard tests...\n" << "\n";

		testBasicOperations();
		testBitManipulation();
		testDistanceFunctions();
		testPawnAttacks();
		testKnightAttacks();
		testKingAttacks();
		testSlidingPieceAttacks();
		testBetweenAndThroughSquares();
		testStringSquareConversion();
		testMaskBitboards();
		testInsideBoard();
		testDirectionCalculation();
		testPrintBitBoard();

		std::cout << "\nBitBoard tests completed." << "\n";
	}

}
