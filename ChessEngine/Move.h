#pragma once
#include <cstdint>
#include <string>
#include <gsl/narrow>

#include "Types.h"

// Move.h - Compact chess move representation (16-bit)
namespace chess
{

class Move {
public:
	// Constructors
	Move() noexcept : m_data(0){}

	// Create a normal move from two squares
	constexpr explicit Move(const uint16_t data) : m_data(data) {}
	constexpr Move(const Square from, const Square to) :
		m_data(gsl::narrow_cast<uint16_t>(from | (to << 6))) {
		assert(isSquare(from) && "Invalid from square");
		assert(isSquare(to) && "Invalid to square");
	}

	// Static factory methods for special moves
	constexpr Move(const Square from, const Square to, const MoveType type,
		const PieceType promotionPiece = KNIGHT) :
		m_data(gsl::narrow_cast<uint16_t>(from | (to << 6) | ((promotionPiece - KNIGHT) << 12) | type)) {
		assert(isSquare(from) && "Invalid from square");
		assert(isSquare(to) && "Invalid to square");
		assert((type != PROMOTION ||
			(promotionPiece >= KNIGHT && promotionPiece <= QUEEN)) &&
			"Invalid promotion piece");
	}

	// Accessor methods
	[[nodiscard]] constexpr Square fromSq() const {
		return static_cast<Square>(m_data & 0x3F);
	}

	[[nodiscard]] constexpr Square toSq() const {
		return static_cast<Square>((m_data >> 6) & 0x3F);
	}

	[[nodiscard]] constexpr int fromToSq() const {
		return m_data & 0xFFF;
	}

	[[nodiscard]] constexpr MoveType moveType() const {
		return static_cast<MoveType>(m_data & (3 << 14));
	}

	[[nodiscard]] constexpr PieceType promotionType() const {
		return static_cast<PieceType>(((m_data >> 12) & 3) + KNIGHT);
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

}