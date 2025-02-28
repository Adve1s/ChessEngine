#pragma once
#include <cstdint>
#include <string>
#include <gsl/narrow>

#include "Types.h"

// Move.h - Compact chess move representation (16-bit)

class Move {
public:
	// Constructors
	Move() noexcept : m_data(0){}

	// Create a normal move from two squares
	constexpr explicit Move(const uint16_t data) : m_data(data) {}
	constexpr Move(const chess::Square from, const chess::Square to) :
		m_data(gsl::narrow_cast<uint16_t>(from | (to << 6))) {
	}

	// Static factory methods for special moves
	constexpr Move(const chess::Square from, const chess::Square to, const chess::MoveType type,
		const chess::PieceType promotionPiece = chess::KNIGHT) :
		m_data(gsl::narrow_cast<uint16_t>(from | (to << 6) | ((promotionPiece - chess::KNIGHT) << 12) | type)) {
	}

	// Accessor methods
	[[nodiscard]] constexpr chess::Square fromSq() const {
		return static_cast<chess::Square>(m_data & 0x3F);
	}

	[[nodiscard]] constexpr chess::Square toSq() const {
		return static_cast<chess::Square>((m_data >> 6) & 0x3F);
	}

	[[nodiscard]] constexpr int fromToSq() const {
		return m_data & 0xFFF;
	}

	[[nodiscard]] constexpr chess::MoveType moveType() const {
		return static_cast<chess::MoveType>(m_data & (3 << 14));
	}

	[[nodiscard]] constexpr chess::PieceType promotionType() const {
		return static_cast<chess::PieceType>(((m_data >> 12) & 3) + chess::KNIGHT);
	}

	// Special move creation
	static constexpr Move null() { return Move(65); }  // Special value(from and to squares are both B1)
	static constexpr Move none() { return Move(0); }   // Empty/invalid move

	// Validation
	[[nodiscard]] constexpr bool validMove() const {
		return none().m_data != m_data && null().m_data != m_data;
	}

	// Conversion to string (for debugging and display)
	[[nodiscard]] std::string toString() const;

	// Comparison operators
	constexpr bool operator==(const Move& m) const = default;

	// Conversion to bool (for if(move) checks)
	constexpr explicit operator bool() const { return m_data != 0; }

	// Raw data access (for debugging, serialization)
	[[nodiscard]] constexpr uint16_t raw() const { return m_data; }

private:
	// Bit layout:
	// bits 0-5: origin square (from) (0-63)
	// bits 6-11:  destination square (to) (0-63)
	// bits 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
	// bits 14-15: move type: normal (0), promotion (1), en passant (2), castling (3)
	uint16_t m_data;
};

// Define a hash function for Move (useful for transposition tables)
struct MoveHash {
	std::size_t operator()(const Move& m) const {
		return m.raw(); // Simple hash using raw data
	}
};