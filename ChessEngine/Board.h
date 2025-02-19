#pragma once
#include <cstdint>
#include <iostream>

using Bitboard = uint64_t; // 64-bit integer for the bitboard

enum PieceType : uint8_t {
	KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NO_PIECE_TYPE
};

enum Color : uint8_t {
	WHITE, BLACK, NO_COLOR
};

class Board
{
public:
	Board();
	void setStartingPosition();
	void printBoard() const;
	static void printSingleBoard(const Bitboard& board);
	void printAllBoards();

private:
	Bitboard m_pieceBitboards[2][6]{}; // 2 players, 6 piece types
};
