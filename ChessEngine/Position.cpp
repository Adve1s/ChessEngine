#include "Position.h"

#include <random>

// Position.cpp - Chess position representation and manipulation

//---------------------------------------------------------------
// Performance: Disable array bounds checking warnings (26446)
// Using direct array access with fixed enum indices (WHITE=0, etc.)
// This is safe due to fixed array sizes matching enum ranges,
// and critical for chess engine performance
//---------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 26446)

namespace chess {

	// Declare zobrist arrays
	namespace zobrist {
		std::array<std::array<HashKey, SQUARE_NB>, PIECE_NB> g_pieceSq;		// Piece-square keys
		std::array<HashKey, FILE_NB> g_enpassant;							// En passant keys
		std::array<HashKey, CASTLING_RIGHT_NB> g_castling;					// Castling rights keys
		HashKey g_side;														// Side to move key
		HashKey g_noPawns;													// No pawns key
	}

	void Position::init()
	{
		// We need a random number generator with a fixed seed(for debugging)
		std::mt19937_64 rng(280304);

		// Initialize piece-square keys
		for (Piece piece = NO_PIECE; piece < PIECE_NB; ++piece)
			for (Square square = A1; square < SQUARE_NB; ++square)
				zobrist::g_pieceSq.at(piece).at(square) = rng();


		// Zero out keys for pawns on promotion ranks (they can't exist there)
		std::fill_n(&zobrist::g_pieceSq.at(W_PAWN).at(A8), 8, 0);
		std::fill_n(&zobrist::g_pieceSq.at(B_PAWN).at(A1), 8, 0);

		// Initialize en passant keys (one per file)
		for (File file = FILE_A; file <= FILE_H; ++file) {
			zobrist::g_enpassant.at(file) = rng();
		}

		// Initialize castling rights keys (16 possible combinations)
		for (int castle = 0; castle < CASTLING_RIGHT_NB; ++castle) {
			zobrist::g_castling.at(castle) = rng();
		}

		// Side to move key (XORed in when it's Black's turn)
		zobrist::g_side = rng();

		// No pawns key (used for pawn hash evaluation)
		zobrist::g_noPawns = rng();
	}

	void Position::clear() noexcept
	{
		// Resetting board representation
		m_colorBB[WHITE] = 0;
		m_colorBB[BLACK] = 0;
		m_pieceBB[ALL_PIECES] = 0;
		m_pieceBB[PAWN] = 0;
		m_pieceBB[KNIGHT] = 0;
		m_pieceBB[BISHOP] = 0;
		m_pieceBB[ROOK] = 0;
		m_pieceBB[QUEEN] = 0;
		m_pieceBB[KING] = 0;

		// Resetting game state
		m_activeColor = WHITE;
		m_castlingRights = NO_CASTLING;
		m_enPassantSquare = NO_SQUARE;

		// Resetting move counters
		m_halfmoveCount = 0;
		m_fullmoveCount = 0;
	}
}
#pragma warning(pop)