#include "Move.h"
#include <sstream>
#include "BitBoard.h"
using chess::Square;
using chess::PieceType;

// Move.cpp - Implementation of chess move operations

// Constructor for basic moves
Move::Move(const Square fromSquare, const Square toSquare, const PieceType piece) noexcept :
	m_data((fromSquare & 0x3F) |						// Store from square in bits 0-5
		((toSquare & 0x3F) << 6) |						// Store to square in bits 6-11
		((static_cast<uint32_t>(piece) & 0x7) << 12) |	// Store piece type in bits 12-14
		(static_cast<uint32_t>(PieceType::NO_PIECE_TYPE) << 15) |	// Set captured piece to NO_PIECE_TYPE
									// Move type defaults to NORMAL (0)
		(static_cast<uint32_t>(PieceType::NO_PIECE_TYPE) << 21)) {	// Set promotion piece to NO_PIECE_TYPE
	assert(isSquare(fromSquare) && "Invalid from square");
	assert(isSquare(toSquare) && "Invalid to square");
	assert(piece >= PieceType::PAWN && piece <= PieceType::KING && "Invalid piece type");
}

// Constructor for captures
Move::Move(const Square fromSquare, const Square toSquare, const PieceType piece, const PieceType capturedPiece) noexcept :
	m_data((fromSquare & 0x3F) |								// Store from square in bits 0-5
		((toSquare & 0x3F) << 6) |								// Store to square in bits 6-11
		((static_cast<uint32_t>(piece) & 0x7) << 12) |			// Store piece type in bits 12-14
		((static_cast<uint32_t>(capturedPiece) & 0x7) << 15) |	// Store captured piece type in bits 15-17
		(static_cast<uint32_t>(chess::CAPTURE) << 18) |			// Set move type to CAPTURE
		(static_cast<uint32_t>(PieceType::NO_PIECE_TYPE) << 21)|// Set promotion piece to NO_PIECE_TYPE
		(1u << 24)) {											// Set capture flag
	assert(isSquare(fromSquare) && "Invalid from square");
	assert(isSquare(toSquare) && "Invalid to square");
	assert(piece >= PieceType::PAWN && piece <= PieceType::KING && "Invalid piece type");
	assert(capturedPiece >= PieceType::PAWN && capturedPiece <= PieceType::KING && "Invalid captured piece type");
}

// Constructor for promotions
Move::Move(const Square fromSquare, const Square toSquare, const PieceType fromPiece, const PieceType toPiece, const bool isCapture, const PieceType capturedPiece) noexcept :
	m_data((fromSquare & 0x3F) |													// Store from square in bits 0-5
		((toSquare & 0x3F) << 6) |													// Store to square in bits 6-11
		((static_cast<uint32_t>(fromPiece) & 0x7) << 12) |							// Store piece type in bits 12-14
		((static_cast<uint32_t>(capturedPiece) & 0x7) << 15) |						// Store captured piece type in bits 15-17
		((static_cast<uint32_t>(isCapture ? chess::PROMOTION_CAPTURE : chess::PROMOTION)) << 18) |// Set move type
		((static_cast<uint32_t>(toPiece) & 0x7) << 21) |							// Store promotion piece type in bits 21-23
		(isCapture ? (1u << 24) : 0) |												// Set the capture flag if needed
		(1u << 25)) {																// Set promotion flag
	assert(isSquare(fromSquare) && "Invalid from square");
	assert(isSquare(toSquare) && "Invalid to square");
	assert(fromPiece >= PieceType::PAWN && fromPiece <= PieceType::KING && "Invalid from piece type");
	assert(toPiece > PieceType::PAWN && toPiece < PieceType::KING && "Invalid to piece type");
	assert(capturedPiece >= PieceType::NO_PIECE_TYPE && capturedPiece <= PieceType::KING && "Invalid captured piece type");
}

// Set the move type and update related flags
void Move::setType(const chess::MoveType type, const PieceType capturedPiece, const PieceType promotionPiece) noexcept
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
	static constexpr std::array<uint32_t, 8>FLAG_PATTERNS = {
		0, // NORMAL
		1 << 24, // CAPTURE (capture flag)
		0, // PAWN_TWO_FORWARD
		1 << 26, // KINGSIDE_CASTLE (castling flag)
		1 << 26, // QUEENSIDE_CASTLE (castling flag)
		1 << 24, // EN_PASSANT (capture flag)
		1 << 25, // PROMOTION (promotion flag)
		(1 << 24) | (1 << 25) // PROMOTION_CAPTURE (capture + promotion flags)
	};

	// Set the flags
	m_data |= FLAG_PATTERNS.at(type);
}

// Convert move to string for output and display
std::string Move::toString() const
{
	std::stringstream ss;

	// Get the square names
	std::string fromStr = "A1"; //TODO: Fix squareToString
	std::string toStr = "A2";   //TODO: Fix squareToString

	// Piece letter (using standard notation: K, Q, R, B, N for pieces, nothing for pawns)
	std::string pieceStr;
	switch (getPiece()) {
	case PieceType::KING:   pieceStr = "K"; break;
	case PieceType::QUEEN:  pieceStr = "Q"; break;
	case PieceType::ROOK:   pieceStr = "R"; break;
	case PieceType::BISHOP: pieceStr = "B"; break;
	case PieceType::KNIGHT: pieceStr = "N"; break;
	case PieceType::PAWN:   pieceStr = "";  break;
	default:     pieceStr = "?"; break;
	}

	// Special case for castling
	if (isCastling()) {
		if (getType() == chess::KINGSIDE_CASTLE) {
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
		case PieceType::QUEEN:  promotionStr = "=Q"; break;
		case PieceType::ROOK:   promotionStr = "=R"; break;
		case PieceType::BISHOP: promotionStr = "=B"; break;
		case PieceType::KNIGHT: promotionStr = "=N"; break;
		default:     promotionStr = "=?"; break;
		}
		ss << promotionStr;
	}

	// Add move type description
	switch (getType()) {
	case chess::CAPTURE:
		ss << " (capture)";
		break;
	case chess::PAWN_TWO_FORWARD:
		ss << " (pawn double push)";
		break;
	case chess::EN_PASSANT:
		ss << " (en passant)";
		break;
	case chess::PROMOTION:
		ss << " (promotion)";
		break;
	case chess::PROMOTION_CAPTURE:
		ss << " (promotion with capture)";
		break;
	default:
		break;
	}

	return ss.str();
}