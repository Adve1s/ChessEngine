#pragma once
#include "types.h"

class Position
{
public:
	Position();

private:
	chess::Bitboard m_pieceBB[chess::PIECE_TYPE_NB]{}; // Bitboards for each piece type (indexed by PieceType)
	chess::Bitboard m_colorBB[chess::COLOR_NB]{};		// Bitboards for each color (indexed by Color)
	chess::Color m_activeColor;
	chess::CastlingRights m_castlingRights;
	chess::Square m_enPassantSquare;
	int m_halfmoveCount;
	int m_fullmoveCount;
};

