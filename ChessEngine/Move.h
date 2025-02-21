#pragma once
#include "Board.h"
#include <string>

// A more detailed enum to describe the type of move
enum MoveType : uint8_t
{
	NORMAL = 0,             // Regular move
	CAPTURE = 1,            // Capturing an opponent's piece
	PAWN_TWO_FORWARD = 2,   // Pawn's initial two-square move
	KINGSIDE_CASTLE = 3,    // King castling to the kingside
	QUEENSIDE_CASTLE = 4,   // King castling to the queenside
	EN_PASSANT = 5,         // En passant capture
	PROMOTION = 6,          // Pawn promotion
	PROMOTION_CAPTURE = 7   // Pawn promotion with capture
};

class Move
{
public:
	// Constructor with basic move information
	Move() : m_data(0) {}

	// Constructor with basic move information
	Move(int fromSquare, int toSquare, PieceType piece);

	// Constructor for captures
	Move(int fromSquare, int toSquare, PieceType piece, PieceType capturedPiece);

	// Constructor for promotions
	Move(int fromSquare, int toSquare, PieceType fromPiece, PieceType toPiece, bool isCapture = false, PieceType capturedPiece = NO_PIECE_TYPE);

	// Getters
	int getFrom() const { return m_data & 0x3F; }
	int getTo() const { return (m_data >> 6) & 0x3F; }
	PieceType getPiece() const { return static_cast<PieceType>((m_data >> 12) & 0x7); }
	MoveType getType() const { return static_cast<MoveType>((m_data >> 18) & 0x7); }
	PieceType getCapturedPiece() const { return static_cast<PieceType>((m_data >> 15) & 0x7); }
	PieceType getPromotionPiece() const { return static_cast<PieceType>((m_data >> 21) & 0x7); }

	// Check if this is a capture move
	bool isCapture() const { return (m_data >> 24) & 1; }
	// Check if this is a promotion move
	bool isPromotion() const { return (m_data >> 25) & 1; }
	// Check if this is a castling move
	bool isCastling() const { return (m_data >> 26) & 1; }

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
// Helper functions - declared here, defined in cpp file
std::string squareToString(int square);
int stringToSquare(const std::string& squareStr);