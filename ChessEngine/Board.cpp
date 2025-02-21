#include "Board.h"
#include <iostream>
#include <sstream>
#include <intrin.h>

// Constructor
Board::Board()
{
	setStartingPosition();
	updateCachedBitboards();
}

void Board::setStartingPosition()
{
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

void Board::setPositionFromFEN(const std::string& fen) {
	// Clear all bitboards
	for (int color = WHITE; color <= BLACK; ++color) {
		for (int pieceType = KING; pieceType <= PAWN; ++pieceType) {
			m_pieceBitboards[color][pieceType] = 0ULL;
		}
	}

	std::istringstream ss(fen);
	std::string boardStr;
	ss >> boardStr; // Get board part of FEN

	int square = 56; // Start at a8

	for (char c : boardStr) {
		if (c == '/') {
			square -= 16; // Move down a rank and back to a-file
		}
		else if (c >= '1' && c <= '8') {
			square += (c - '0'); // Skip empty squares
		}
		else {
			// Place a piece
			Color color = (c >= 'a') ? BLACK : WHITE;
			Bitboard squareBit = 1ULL << square;

			switch (std::toupper(c)) {
			case 'K': m_pieceBitboards[color][KING] |= squareBit; break;
			case 'Q': m_pieceBitboards[color][QUEEN] |= squareBit; break;
			case 'R': m_pieceBitboards[color][ROOK] |= squareBit; break;
			case 'B': m_pieceBitboards[color][BISHOP] |= squareBit; break;
			case 'N': m_pieceBitboards[color][KNIGHT] |= squareBit; break;
			case 'P': m_pieceBitboards[color][PAWN] |= squareBit; break;
			default:
				// Invalid piece character - could add error handling here
				square--; // Don't increment square for invalid characters
				break;
			}

			square++; // Move to next square
		}
	}

	// Parse rest of FEN for turn, castling rights, etc.
	// ...

	// Update cached bitboards after setting up the position
	updateCachedBitboards();
}

void Board::updateCachedBitboards()
{
	// Clear the cached bitboards
	m_occupiedSquares = 0ULL;
	m_colorPieces[WHITE] = 0ULL;
	m_colorPieces[BLACK] = 0ULL;

	// Update color pieces bitboards and occupied squares
	for (int color = WHITE; color <= BLACK; ++color) {
		for (int pieceType = KING; pieceType <= PAWN; ++pieceType) {
			m_colorPieces[color] |= m_pieceBitboards[color][pieceType];
		}
		// Update occupied squares
		m_occupiedSquares |= m_colorPieces[color];
	}
}

bool Board::isSquareEmpty(int square) const
{
	Bitboard squareMask = 1ULL << square;
	return !(m_occupiedSquares & squareMask);
}

bool Board::isSquareOccupiedBy(int square, Color color) const
{
	Bitboard squareMask = 1ULL << square;
	return (m_colorPieces[color] & squareMask);
}

// Get the least significant bit position (first set bit)
static int getLSBIndex(Bitboard board) {
	if (board == 0) return -1;
	unsigned long index;
	_BitScanForward64(&index, board);

	return static_cast<int>(index);
}

// Get and clear the least significant bit
static int popLSB(Bitboard& board) {
	if (board == 0) return -1;

	unsigned long index;
	_BitScanForward64(&index, board);

	// Clear the least significant bit using a bitwise operation
	// (x & (x-1)) clears the least significant set bit
	board &= (board - 1);

	return static_cast<int>(index);
}

// Count the number of set bits (pieces on the board)
static int countBits(Bitboard board) {
	return static_cast<int>(__popcnt64(board));
}

void Board::printBoard() const
{
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

void Board::printSingleBoard(const Bitboard& board)
{
	for (int rank = 7; rank >= 0; --rank) {
		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			std::cout << ((board >> square) & 1) << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void Board::printAllBoards()
{
	for (auto& playerBoards : m_pieceBitboards) {
		for (auto& pieceBoard : playerBoards) {
			printSingleBoard(pieceBoard);
		}
	}
}