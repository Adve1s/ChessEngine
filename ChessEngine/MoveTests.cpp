#include <iostream>
#include <cassert>
#include <unordered_set>
#include "Move.h"
#include "Types.h"

// MoveTests.cpp - Tests for the compact 16-bit Move class

namespace chess::tests {

	// Test constructors and basic accessors
	void testConstructors() {
		// Default constructor
		const Move defaultMove;
		// We can only check that it exists, but can't make assumptions about its value
		report("Default constructor exists", true);

		// Let's check if it equals none() which would indicate zero-initialization
		if (defaultMove.raw() == Move::none().raw()) {
			report("Default constructor creates none() move", true);
		}
		else {
			std::cout << "Note: Default constructor doesn't zero-initialize (raw value: "
				<< defaultMove.raw() << ")" << "\n";
		}

		// Raw data constructor
		constexpr uint16_t rawData = 0x1234;
		constexpr Move rawMove(rawData);
		report("Raw data constructor", rawMove.raw() == rawData);

		// From-To constructor
		constexpr Square from = E2;
		constexpr Square to = E4;
		constexpr Move basicMove(from, to);
		constexpr bool fromToOK = basicMove.fromSq() == from && basicMove.toSq() == to;
		report("From-To constructor", fromToOK);

		// Type constructor with promotion
		constexpr Move promMove(E7, E8, PROMOTION, QUEEN);
		constexpr bool promOK = promMove.fromSq() == E7 &&
			promMove.toSq() == E8 &&
			promMove.moveType() == PROMOTION &&
			promMove.promotionType() == QUEEN;
		report("Type constructor with promotion", promOK);
	}

	// Test special move creation
	void testSpecialMoves() {
		// Null move
		constexpr Move nullMove = Move::null();
		report("Null move creation", nullMove.raw() == 65);
		report("Null move validation", !nullMove.validMove());

		// None move
		constexpr Move noneMove = Move::none();
		report("None move creation", noneMove.raw() == 0);
		report("None move validation", !noneMove.validMove());
	}

	// Test move types
	void testMoveTypes() {
		// Normal move
		constexpr Move normalMove(E2, E4);
		report("Normal move type", normalMove.moveType() == 0);

		// Promotion move
		constexpr Move promMove(E7, E8, PROMOTION, QUEEN);
		report("Promotion move type", promMove.moveType() == PROMOTION);
		report("Promotion piece type", promMove.promotionType() == QUEEN);

		// En Passant move
		constexpr Move epMove(E5, D6, EN_PASSANT);
		report("En Passant move type", epMove.moveType() == EN_PASSANT);

		// Castling move
		constexpr Move castleMove(E1, G1, CASTLING);
		report("Castling move type", castleMove.moveType() == CASTLING);
	}

	// Test from-to access and composition
	void testFromToAccess() {
		constexpr Square from = D2;
		constexpr Square to = D4;
		constexpr Move move(from, to);

		report("From square access", move.fromSq() == from);
		report("To square access", move.toSq() == to);

		// Test fromToSq combined value
		constexpr int expectedFromTo = from | (to << 6);
		report("FromTo combined access", move.fromToSq() == expectedFromTo);
	}

	// Test boolean conversion operator
	void testBoolConversion() {
		constexpr Move validMove(E2, E4);
		report("Valid move converts to true", static_cast<bool>(validMove));

		constexpr Move emptyMove = Move::none();
		report("Empty move converts to false", !static_cast<bool>(emptyMove));
	}

	// Test equality operator
	void testEqualityOperator() {
		constexpr Move move1(E2, E4);
		constexpr Move move2(E2, E4);
		constexpr Move move3(D2, D4);

		report("Equal moves compare equal", move1 == move2);
		report("Different moves compare unequal", !(move1 == move3));

		// Create same move with different raw values
		constexpr Move specialMove1(E7, E8, PROMOTION, QUEEN);
		constexpr uint16_t rawValue = specialMove1.raw();
		constexpr Move specialMove2(rawValue);

		report("Same special moves equal", specialMove1 == specialMove2);
	}

	// Test MoveHash
	void testMoveHash() {
		constexpr Move move(E2, E4);
		constexpr MoveHash hasher;
		const size_t hash = hasher(move);

		report("Hash function returns raw value", hash == move.raw());

		// Test hash in a container
		std::unordered_set<Move, MoveHash> moveSet;
		moveSet.insert(move);
		moveSet.insert(Move(D2, D4));

		const bool contains = moveSet.contains(move);
		const bool doesntContain = !moveSet.contains(Move(E7, E8));

		report("Move can be used in hash containers", contains && doesntContain);
	}

	// Test all possible promotion pieces
	void testPromotionPieces() {
		// Knight promotion
	 	constexpr Move knightProm(E7, E8, PROMOTION, KNIGHT);
		report("Knight promotion type", knightProm.promotionType() == KNIGHT);

		// Bishop promotion
		constexpr Move bishopProm(E7, E8, PROMOTION, BISHOP);
		report("Bishop promotion type", bishopProm.promotionType() == BISHOP);

		// Rook promotion
		constexpr Move rookProm(E7, E8, PROMOTION, ROOK);
		report("Rook promotion type", rookProm.promotionType() == ROOK);

		// Queen promotion
		constexpr Move queenProm(E7, E8, PROMOTION, QUEEN);
		report("Queen promotion type", queenProm.promotionType() == QUEEN);
	}

	// Test edge cases
	void testEdgeCases() {
		// Corner squares
		constexpr Move cornerMove(A1, H8);
		constexpr bool cornerOK = cornerMove.fromSq() == A1 && cornerMove.toSq() == H8;
		report("Corner squares move", cornerOK);

		// Same square move (usually invalid in chess but should work as a Move object)
		constexpr Move sameSquare(E4, E4);
		constexpr bool sameOK = sameSquare.fromSq() == E4 && sameSquare.toSq() == E4;
		report("Same square move", sameOK);

		// Test validMove() for regular moves
		report("Regular move is valid", cornerMove.validMove());
	}

	// Test bit manipulation edge cases
	void testMoveBitManipulation() {
		// Test squares at bit boundaries
		constexpr auto from = static_cast<Square>(63); // Last square (H8)
		constexpr auto to = static_cast<Square>(0);    // First square (A1)

		constexpr Move edgeMove(from, to);
		constexpr bool edgeOK = edgeMove.fromSq() == from && edgeMove.toSq() == to;
		report("Edge square bit manipulation", edgeOK);

		// Test promotion piece at bit boundaries
		constexpr Move edgeProm(E7, E8, PROMOTION, QUEEN);
		report("Edge promotion bit manipulation", edgeProm.promotionType() == QUEEN);
	}

	// Run all tests
	void runAllMoveTests() {
		std::cout << "===== Running Move Class Tests =====" << "\n";

		testConstructors();
		testSpecialMoves();
		testMoveTypes();
		testFromToAccess();
		testBoolConversion();
		testEqualityOperator();
		testMoveHash();
		testPromotionPieces();
		testEdgeCases();
		testMoveBitManipulation();

		std::cout << "===== Move Class Tests Complete =====" << "\n";
	}

} // namespace chess::tests

// No standalone main function - this would conflict with your existing main
// Instead, call chess::tests::runAllMoveTests() from your existing main function