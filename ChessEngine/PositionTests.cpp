#include "PositionTests.h"
#include <iostream>
#include <set>

#include "BitBoard.h"
#include "Position.h"
#include "Types.h"

namespace chess::tests
{
	// Test piece-square keys initialization
	void testPieceSquareKeys() {
		bool success = true;
		int nonZeroCount = 0;

		// Check all piece-square combinations
		for (Piece piece = NO_PIECE; piece < PIECE_NB; ++piece) {
			for (Square square = A1; square < SQUARE_NB; ++square) {
				const HashKey key = zobrist::g_pieceSq.at(piece).at(square);

				// Special cases: pawns on promotion ranks should be zero
				const bool promotedPawn = (piece == W_PAWN && rankOf(square) == RANK_8) ||
					(piece == B_PAWN && rankOf(square) == RANK_1);

				if (promotedPawn && (key != 0)) {
					std::cout << "Pawn on promotion rank should have zero key: ";
					std::cout << "Piece: " << piece << ", Square: " << squareToString(square) << "\n";
					success = false;
				}
				else if (key == 0 && !promotedPawn && piece != NO_PIECE) {
					// Other pieces should not have non-zero keys
					std::cout << "Non-zero key expected: ";
					std::cout << "Piece: " << piece << ", Square: " << squareToString(square) << "\n";
					success = false;
				}
				else if (key != 0) {
					nonZeroCount++;
				}
			}
		}

		std::cout << "Non-zero keys: " << nonZeroCount << " out of " << (static_cast<int>(PIECE_NB) * static_cast<int>(SQUARE_NB)) << "\n";
		report("Piece-square keys", success);
	}

	// Test en passant keys initialization
	void testEnPassantKeys() {
		bool success = true;

		// Check all file en passant keys
		for (File file = FILE_A; file <= FILE_H; ++file) {
			if (zobrist::g_enpassant.at(file) == 0) {
				std::cout << "En passant key for file " << file << " is zero" << "\n";
				success = false;
			}
		}

		report("En passant keys", success);
	}

	// Test castling rights keys initialization
	void testCastlingKeys() {
		bool success = true;

		// Check all castling rights keys
		for (int castle = 0; castle < CASTLING_RIGHT_NB; ++castle) {
			if (zobrist::g_castling.at(castle) == 0 && castle != 0) {
				// castle == 0 means NO_CASTLING, which could reasonably be zero
				std::cout << "Castling rights key for value " << castle << " is zero" << "\n";
				success = false;
			}
		}

		report("Castling rights keys", success);
	}

	// Test side to move and noPawns keys
	void testMiscKeys() {
		bool success = true;

		// Side to move key should be non-zero
		if (zobrist::g_side == 0) {
			std::cout << "Side to move key is zero" << "\n";
			success = false;
		}

		// No pawns key should be non-zero
		if (zobrist::g_noPawns == 0) {
			std::cout << "No pawns key is zero" << "\n";
			success = false;
		}

		report("Miscellaneous keys", success);
	}

	// Test basic uniqueness of keys
	void testKeyUniqueness() {
		std::set<HashKey> keySet;

		// Add a sample of keys to check they're different
		keySet.insert(zobrist::g_pieceSq.at(W_PAWN).at(A2));
		keySet.insert(zobrist::g_pieceSq.at(W_PAWN).at(B2));
		keySet.insert(zobrist::g_pieceSq.at(B_PAWN).at(A7));
		keySet.insert(zobrist::g_pieceSq.at(W_KNIGHT).at(B1));
		keySet.insert(zobrist::g_enpassant.at(FILE_E));
		keySet.insert(zobrist::g_castling.at(WHITE_OO));
		keySet.insert(zobrist::g_side);

		const bool success = (keySet.size() == 7);
		report("Key uniqueness test", success);
	}

	// Run all Position tests
	void runAllPositionTests() {
		std::cout << "Running Position tests...\n" << "\n";

		testPieceSquareKeys();
		testEnPassantKeys();
		testCastlingKeys();
		testMiscKeys();
		testKeyUniqueness();

		std::cout << "\nPosition tests completed." << "\n";
	}
}