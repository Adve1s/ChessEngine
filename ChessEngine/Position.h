#pragma once
#include "types.h"
#include <array>


// Position.h - Chess position representation and manipulation

class Position
{
public:
	// Temporary test function
	void printHi() const;
private:
	// Board representation using bitboards
	std::array < chess::Bitboard, chess::PIECE_TYPE_NB> m_pieceBB{};  // Pieces by type
	std::array < chess::Bitboard, chess::COLOR_NB> m_colorBB{};      // Pieces by color

	// Game state
	chess::Color m_activeColor = chess::WHITE;                       // Side to move
	chess::CastlingRights m_castlingRights = chess::ANY_CASTLING;    // Available castling options
	chess::Square m_enPassantSquare = chess::NO_SQUARE;              // En passant target square if available

	// Move counters
	int m_halfmoveCount = 0;    // Halfmove clock (for 50-move rule)
	int m_fullmoveCount = 0;    // Fullmove counter (starts at 1, incremented after Black's move)

};

