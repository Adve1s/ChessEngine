#pragma once
#include <string>
#include "Types.h"

// Move.h - Chess move representation and operations

class Move
{
public:
	// Default constructor - creates an empty/null move
	Move() noexcept : m_data(0) {}

	// Constructor for basic moves
	Move(chess::Square fromSquare, chess::Square toSquare, chess::PieceType piece) noexcept;

	// Constructor for captures
	Move(chess::Square fromSquare, chess::Square toSquare, chess::PieceType piece, chess::PieceType capturedPiece) noexcept;

	// Constructor for promotions (with optional capture)
	Move(chess::Square fromSquare, chess::Square toSquare, chess::PieceType fromPiece, chess::PieceType toPiece, bool isCapture = false, chess::PieceType capturedPiece = chess::NO_PIECE_TYPE) noexcept;

	// Getters for move information
	int getFrom() const noexcept { return m_data & 0x3F; }
	int getTo() const noexcept { return (m_data >> 6) & 0x3F; }
	chess::PieceType getPiece() const noexcept { return static_cast<chess::PieceType>((m_data >> 12) & 0x7); }
	chess::MoveType getType() const noexcept { return static_cast<chess::MoveType>((m_data >> 18) & 0x7); }
	chess::PieceType getCapturedPiece() const noexcept { return static_cast<chess::PieceType>((m_data >> 15) & 0x7); }
	chess::PieceType getPromotionPiece() const noexcept { return static_cast<chess::PieceType>((m_data >> 21) & 0x7); }

	// Move property checks
	bool isCapture() const noexcept { return isCaptureFlag(m_data); }
	bool isPromotion() const noexcept { return isPromotionFlag(m_data); }
	bool isCastling() const { return isCastlingFlag(m_data); }

	// Static flag checking methods
	constexpr static bool isCaptureFlag(const uint32_t moveData) noexcept { return (moveData >> 24) & 1; }
	constexpr static bool isPromotionFlag(const uint32_t moveData) noexcept { return (moveData >> 25) & 1; }
	constexpr static bool isCastlingFlag(const uint32_t moveData) { return (moveData >> 26) & 1; }

	// Comparison operators
	friend bool operator==(const Move& lhs, const Move& rhs) noexcept {
		// Quick check - if the raw data is identical, moves are identical
		if (lhs.m_data == rhs.m_data) return true;

		// Otherwise, check essential properties
		return (lhs.getFrom() == rhs.getFrom() &&
			lhs.getTo() == rhs.getTo() &&
			lhs.getPiece() == rhs.getPiece() &&
			lhs.getType() == rhs.getType() &&
			// Only check promotion piece if either move is a promotion
			((!lhs.isPromotion() && !rhs.isPromotion()) ||
				lhs.getPromotionPiece() == rhs.getPromotionPiece()));
	}

	friend bool operator!=(const Move& lhs, const Move& rhs) noexcept {
		return !(lhs == rhs);
	}

	// Set move type (and update flags)
	void setType(chess::MoveType type, chess::PieceType capturedPiece = chess::NO_PIECE_TYPE, chess::PieceType promotionPiece = chess::NO_PIECE_TYPE) noexcept;

	// Convert move to string representation
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
