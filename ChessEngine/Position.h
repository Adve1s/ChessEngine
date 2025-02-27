#pragma once
#include "types.h"
#include <array>

class Position
{
public:
	void printHi() const;
private:
	std::array < chess::Bitboard, chess::PIECE_TYPE_NB> m_pieceBB {}; // Bitboards for each piece type (indexed by PieceType)
	std::array < chess::Bitboard, chess::COLOR_NB> m_colorBB {};		// Bitboards for each color (indexed by Color)
	chess::Color m_activeColor = chess::WHITE;
	chess::CastlingRights m_castlingRights = chess::ANY_CASTLING;
	chess::Square m_enPassantSquare = chess::NO_SQUARE;
	int m_halfmoveCount = 0;
	int m_fullmoveCount = 0;
};

