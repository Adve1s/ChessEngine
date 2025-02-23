#pragma once
#include "types.h"

using namespace chess;

class Position
{
public:
	Position();

private:
	Bitboard m_pieceBB[PIECE_TYPE_NB]{}; // Bitboards for each piece type (indexed by PieceType)
	Bitboard m_colorBB[COLOR_NB]{};		// Bitboards for each color (indexed by Color)
	Color m_activeColor;
	CastlingRights m_castlingRights;
	Square m_enPassantSquare;
	int m_halfmoveCount;
	int m_fullmoveCount;
};

