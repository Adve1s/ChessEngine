#pragma once
#include <cstdint>
#include <string>

using Bitboard = uint64_t; // 64-bit integer for the bitboard

enum PieceType : uint8_t
{
	KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NO_PIECE_TYPE
};

enum Color : uint8_t
{
	WHITE, BLACK, NO_COLOR
};

enum Square : uint8_t
{
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8
};

class Board
{
public:
	Board();
	void setStartingPosition();
	void setPositionFromFEN(const std::string& fen);
	void updateCachedBitboards();
	bool isSquareEmpty(int square) const;
	bool isSquareOccupiedBy(int square, Color color) const;
	static Bitboard squareToBitboard(int square) {return 1ULL << square;}
	static int getLSBIndex(Bitboard board);
	static int popLSB(Bitboard& board);
	static int countBits(Bitboard board);
	void printBoard() const;
	static void printSingleBoard(const Bitboard& board);
	void printAllBoards();

private:
	Bitboard m_pieceBitboards[2][6]{}; // 2 players, 6 piece types
	Bitboard m_occupiedSquares;     // All occupied squares
	Bitboard m_colorPieces[2];      // All pieces for each color
};
