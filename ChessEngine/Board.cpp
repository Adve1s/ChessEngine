using namespace std;
#include "Board.h"
#include <iostream>


// Constructor
Board::Board() {
	setStartingPosition();
}

void Board::setStartingPosition() {
	// Set up white pieces
	m_pieceBitboards[WHITE][PAWN] = 0x000000000000FF00ULL; // 2nd rank
	m_pieceBitboards[WHITE][KNIGHT] = 0x0000000000000042ULL; // b1, g1
	m_pieceBitboards[WHITE][BISHOP] = 0x0000000000000024ULL; // c1, f1
	m_pieceBitboards[WHITE][ROOK] = 0x0000000000000081ULL; // a1, h1
	m_pieceBitboards[WHITE][QUEEN] = 0x0000000000000008ULL; // d1
	m_pieceBitboards[WHITE][KING] = 0x0000000000000010ULL; // e1

	// Set up black pieces
	m_pieceBitboards[BLACK][PAWN] = 0x00FF000000000000ULL; // 7th rank
	m_pieceBitboards[BLACK][KNIGHT] = 0x4200000000000000ULL; // b8, g8
	m_pieceBitboards[BLACK][BISHOP] = 0x2400000000000000ULL; // c8, f8
	m_pieceBitboards[BLACK][ROOK] = 0x8100000000000000ULL; // a8, h8
	m_pieceBitboards[BLACK][QUEEN] = 0x0800000000000000ULL; // d8
	m_pieceBitboards[BLACK][KING] = 0x1000000000000000ULL; // e8
}

void Board::printBoard() const {
	const char* pieceSymbols[2][6] = {
		{"K", "Q", "R", "B", "N", "P"},  // White pieces
		{"k", "q", "r", "b", "n", "p"}   // Black pieces
	};

	std::cout << "  +-----------------+\n";

	for (int rank = 7; rank >= 0; --rank) {
		std::cout << (rank + 1) << " | ";

		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			uint64_t squareMask = 1ULL << square;
			bool pieceFound = false;

			// Check each piece bitboard for this square
			for (int color = WHITE; color <= BLACK; ++color) {
				for (int pieceType = KING; pieceType <= PAWN; ++pieceType) {
					if (m_pieceBitboards[color][pieceType] & squareMask) {
						std::cout << pieceSymbols[color][pieceType] << " ";
						pieceFound = true;
						break;  // Found a piece on this square
					}
				}
				if (pieceFound) break;  // No need to check other colors
			}

			// Empty square
			if (!pieceFound) {
				// Alternate colors for empty squares
				char emptySquare = ((rank + file) % 2 == 0) ? '·' : ' ';
				std::cout << emptySquare << " ";
			}
		}

		std::cout << "|\n";
	}

	std::cout << "  +-----------------+\n";
	std::cout << "    a b c d e f g h\n";
}

void Board::printSingleBoard(const Bitboard& board) {

	for (int rank = 7; rank >= 0; --rank) {
		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			cout << ((board >> square) & 1) << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

void Board::printAllBoards() {
	for (auto& playerBoards : m_pieceBitboards) {
		for (auto& pieceBoard : playerBoards) {
			printSingleBoard(pieceBoard);
		}
	}
}