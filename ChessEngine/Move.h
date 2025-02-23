#pragma once
#include <string>
#include "types.h"

using namespace chess;

class Move
{
public:
	// Constructor with basic move information
	Move() : m_data(0) {}

	// Constructor with basic move information
	Move(Square fromSquare, Square toSquare, PieceType piece);

	// Constructor for captures
	Move(Square fromSquare, Square toSquare, PieceType piece, PieceType capturedPiece);

	// Constructor for promotions
	Move(Square fromSquare, Square toSquare, PieceType fromPiece, PieceType toPiece, bool isCapture = false, PieceType capturedPiece = NO_PIECE_TYPE);

	// Getters
	int getFrom() const { return m_data & 0x3F; }
	int getTo() const { return (m_data >> 6) & 0x3F; }
	PieceType getPiece() const { return static_cast<PieceType>((m_data >> 12) & 0x7); }
	MoveType getType() const { return static_cast<MoveType>((m_data >> 18) & 0x7); }
	PieceType getCapturedPiece() const { return static_cast<PieceType>((m_data >> 15) & 0x7); }
	PieceType getPromotionPiece() const { return static_cast<PieceType>((m_data >> 21) & 0x7); }

	// Object-oriented methods (for working with Move objects)
	bool isCapture() const { return isCaptureFlag(m_data); }
	bool isPromotion() const { return isPromotionFlag(m_data); }
	bool isCastling() const { return isCastlingFlag(m_data); }

	// Static utility methods (for working with raw move data)
	static bool isCaptureFlag(const uint32_t moveData) { return (moveData >> 24) & 1; }
	static bool isPromotionFlag(const uint32_t moveData) { return (moveData >> 25) & 1; }
	static bool isCastlingFlag(const uint32_t moveData) { return (moveData >> 26) & 1; }

	bool operator==(const Move& other) const;
	bool operator!=(const Move& other) const {return !(*this == other);}
	// Set the move type
	void setType(MoveType type, PieceType capturedPiece = NO_PIECE_TYPE, PieceType promotionPiece = NO_PIECE_TYPE);

	// For printing/debugging
	std::string toString() const;

private:
	// Bit layout for m_data (32 bits):
	// bits 0-5:   from square (0-63)
	// bits 6-11:  to square (0-63)
	// bits 12-14: piece type (0-5)
	// bits 15-17: captured piece type (0-5, or 7 for none)
	// bits 18-20: move type (0-7)
	// bits 21-23: promotion piece type (0-5, only used for promotions)
	// bits 24-31: reserved for future use
	uint32_t m_data;
};