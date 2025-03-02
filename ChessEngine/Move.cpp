#include "Move.h"
#include "BitBoard.h"
#include <sstream>

namespace chess {
	// Convert a move to a human-readable string
	std::string Move::toString() const {
		if (*this == none())
			return "none";
		if (*this == null())
			return "null";

		// Get square names using your existing utility
		std::string from = squareToString(fromSq());
		std::string to = squareToString(toSq());

		std::stringstream ss;

		// Handle castling moves specially
		if (moveType() == CASTLING) {
			// Determine kingside or queenside based on the squares
			const bool isKingside = toSq() > fromSq();
			ss << (isKingside ? "O-O" : "O-O-O");
			return ss.str();
		}

		// Add origin and destination
		ss << from << to;

		// Add promotion piece if applicable
		if (moveType() == PROMOTION) {
			char promotionChar = ' ';
			switch (promotionType()) {
			case KNIGHT: promotionChar = 'n'; break;
			case BISHOP: promotionChar = 'b'; break;
			case ROOK:   promotionChar = 'r'; break;
			case QUEEN:  promotionChar = 'q'; break;
			default: break;
			}
			ss << promotionChar;
		}

		return ss.str();
	}
}