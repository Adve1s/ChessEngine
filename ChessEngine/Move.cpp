#include "Move.h"
#include <sstream>

// Constructor with basic move information
Move::Move(int fromSquare, int toSquare, PieceType piece) :
	m_data((fromSquare & 0x3F) |						// Store from square in bits 0-5
		((toSquare & 0x3F) << 6) |						// Store to square in bits 6-11
		((static_cast<uint32_t>(piece) & 0x7) << 12) |	// Store piece type in bits 12-14
		(static_cast<uint32_t>(NO_PIECE_TYPE) << 15) |	// Set captured piece to NO_PIECE_TYPE (6) in bits 15-17
														// Set move type to NORMAL (0) in bits 18-20 (This is already 0 since we initialized m_data to 0)
		(static_cast<uint32_t>(NO_PIECE_TYPE) << 21)) {	// Set promotion piece to NO_PIECE_TYPE (6) in bits 21-23
	// All initialization is done
}

// Constructor for captures
Move::Move(int fromSquare, int toSquare, PieceType piece, PieceType capturedPiece) :
	m_data((fromSquare & 0x3F) |								// Store from square in bits 0-5
		((toSquare & 0x3F) << 6) |								// Store to square in bits 6-11
		((static_cast<uint32_t>(piece) & 0x7) << 12) |			// Store piece type in bits 12-14
		((static_cast<uint32_t>(capturedPiece) & 0x7) << 15) |	// Store captured piece type in bits 15-17
		(static_cast<uint32_t>(CAPTURE) << 18) |				// Set move type to CAPTURE (1) in bits 18-20
		(static_cast<uint32_t>(NO_PIECE_TYPE) << 21) |			// Set promotion piece to NO_PIECE_TYPE (6) in bits 21-23
		(1u << 24)) {											// Capture flag
	// All initialization is done
}

// Constructor for promotions
Move::Move(int fromSquare, int toSquare, PieceType fromPiece, PieceType toPiece, bool isCapture, PieceType capturedPiece) :
	m_data((fromSquare & 0x3F) |													// Store from square in bits 0-5
		((toSquare & 0x3F) << 6) |													// Store to square in bits 6-11
		((static_cast<uint32_t>(fromPiece) & 0x7) << 12) |							// Store piece type in bits 12-14
		((static_cast<uint32_t>(capturedPiece) & 0x7) << 15) |						// Store captured piece type in bits 15-17
		((static_cast<uint32_t>(isCapture ? PROMOTION_CAPTURE : PROMOTION)) << 18) |// Set move type to PROMOTION_CAPTURE (7) in bits 18-20
		((static_cast<uint32_t>(toPiece) & 0x7) << 21) |							// Store promotion piece type in bits 21-23
		(isCapture ? (1u << 24) : 0) |												// Set the capture flag in bit 24
		(1u << 25)) {																// Promotion flag
	// All initialization is done
}

bool Move::operator==(const Move& other) const {
	// Quick check - if the raw data is identical, moves are identical
	if (m_data == other.m_data) return true;

	// Otherwise, check essential properties
	return (getFrom() == other.getFrom() &&
		getTo() == other.getTo() &&
		getPiece() == other.getPiece() &&
		getType() == other.getType() &&
		// Only check promotion piece if either move is a promotion
		((!isPromotion() && !other.isPromotion()) ||
			getPromotionPiece() == other.getPromotionPiece()));
}

void Move::setType(MoveType type, PieceType capturedPiece, PieceType promotionPiece)
{
	// Clear the old move type bits (bits 18-20)
	m_data &= ~(0x7u << 18);

	// Set the new move type bits
	m_data |= ((static_cast<uint32_t>(type) & 0x7) << 18);

	// Clear all flag bits (bits 24-26)
	m_data &= ~(0x7u << 24);

	// Clear captured piece bits (bits 15-17)
	m_data &= ~(0x7u << 15);
	// Clear promotion piece bits (bits 21-23)
	m_data &= ~(0x7u << 21);

	// Set captured piece and promotion piece based on parameters
	m_data |= ((static_cast<uint32_t>(capturedPiece) & 0x7) << 15);
	m_data |= ((static_cast<uint32_t>(promotionPiece) & 0x7) << 21);

	// Flag patterns for each move type (positioned at bits 24-26)
	static const uint32_t flagPatterns[8] = {
		0,                  // NORMAL
		1 << 24,            // CAPTURE (capture flag)
		0,                  // PAWN_TWO_FORWARD
		1 << 26,            // KINGSIDE_CASTLE (castling flag)
		1 << 26,            // QUEENSIDE_CASTLE (castling flag)
		1 << 24,            // EN_PASSANT (capture flag)
		1 << 25,            // PROMOTION (promotion flag)
		(1 << 24) | (1 << 25) // PROMOTION_CAPTURE (capture + promotion flags)
	};

	// Set the flags
	m_data |= flagPatterns[type];
}

// Convert move to string for output
std::string Move::toString() const
{
	std::stringstream ss;

	// Get the square names
	std::string fromStr = squareToString(getFrom());
	std::string toStr = squareToString(getTo());

	// Piece letter (using standard notation: K, Q, R, B, N for pieces, nothing for pawns)
	std::string pieceStr;
	switch (getPiece()) {
	case KING:   pieceStr = "K"; break;
	case QUEEN:  pieceStr = "Q"; break;
	case ROOK:   pieceStr = "R"; break;
	case BISHOP: pieceStr = "B"; break;
	case KNIGHT: pieceStr = "N"; break;
	case PAWN:   pieceStr = "";  break;
	default:     pieceStr = "?"; break;
	}

	// Special case for castling
	if (isCastling()) {
		if (getType() == KINGSIDE_CASTLE) {
			return "O-O (Kingside Castle)";
		}
		else {
			return "O-O-O (Queenside Castle)";
		}
	}

	// Build the move string
	ss << pieceStr << fromStr;

	// Add 'x' for captures
	if (isCapture()) {
		ss << "x";
	}
	else {
		ss << "-";
	}

	ss << toStr;

	// Add promotion piece
	if (isPromotion()) {
		std::string promotionStr;
		switch (getPromotionPiece()) {
		case QUEEN:  promotionStr = "=Q"; break;
		case ROOK:   promotionStr = "=R"; break;
		case BISHOP: promotionStr = "=B"; break;
		case KNIGHT: promotionStr = "=N"; break;
		default:     promotionStr = "=?"; break;
		}
		ss << promotionStr;
	}

	// Add move type description
	switch (getType()) {
	case CAPTURE:
		ss << " (capture)";
		break;
	case PAWN_TWO_FORWARD:
		ss << " (pawn double push)";
		break;
	case EN_PASSANT:
		ss << " (en passant)";
		break;
	case PROMOTION:
		ss << " (promotion)";
		break;
	case PROMOTION_CAPTURE:
		ss << " (promotion with capture)";
		break;
	case KINGSIDE_CASTLE:
		// Already handled earlier
		break;
	case QUEENSIDE_CASTLE:
		// Already handled earlier
		break;
	default:
		break;
	}

	return ss.str();
}

// Convert a square number (0-63) to algebraic notation (a1, h8, etc.)
std::string squareToString(int square)
{
	char file = 'a' + (square % 8);    // Horizontal position (a-h)
	char rank = '1' + (square / 8);    // Vertical position (1-8)

	return std::string(1, file) + std::string(1, rank);
}

// Convert algebraic notation to a square number
int stringToSquare(const std::string& squareStr)
{
	if (squareStr.length() < 2) {
		return -1; // Invalid square string
	}

	int file = squareStr[0] - 'a';
	int rank = squareStr[1] - '1';

	if (file < 0 || file > 7 || rank < 0 || rank > 7) {
		return -1; // Invalid coordinates
	}

	return rank * 8 + file;
}