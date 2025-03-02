#pragma once
#include "types.h"
#include <array>

// Position.h - Chess position representation and manipulation

namespace chess {

	// Declare zobrist arrays
	namespace zobrist {
		extern std::array<std::array<HashKey, SQUARE_NB>, PIECE_NB> g_pieceSq;  // Piece-square keys
		extern std::array<HashKey, FILE_NB> g_enpassant;						// En passant keys
		extern std::array<HashKey, CASTLING_RIGHT_NB> g_castling;			    // Castling rights keys
		extern HashKey g_side;													// Side to move key
		extern HashKey g_noPawns;												// No pawns key
	}

	class Position
	{
	public:

		Position() = default;
		static void init();
		void clear() noexcept;

		void putPiece(Piece piece, Square square);
		void removePiece(Square square);
		void movePiece(Square from, Square to);
	private:
		// Board representation using bitboards
		std::array <Piece, SQUARE_NB> m_board{};			// Whole board
		std::array <Bitboard, PIECE_TYPE_NB> m_pieceBB{};	// Pieces by type
		std::array <Bitboard, COLOR_NB> m_colorBB{};		// Pieces by color
		std::array<int, PIECE_NB> m_pieceCount{};           // Number of each piece

		// Castling arrays
		std::array<int, SQUARE_NB> m_castlingRightsMask{};
		std::array<Square, CASTLING_RIGHT_NB> m_castlingRookSquare{};
		std::array<Bitboard, CASTLING_RIGHT_NB> m_castlingPath{};

		// Game state
		Color m_activeColor = WHITE;                       // Side to move
		CastlingRights m_castlingRights = NO_CASTLING;	   // Available castling options
		Square m_enPassantSquare = NO_SQUARE;              // En passant target square if available

		// Move counters
		int m_halfmoveCount = 0;    // Halfmove clock (for 50-move rule)
		int m_fullmoveCount = 1;    // Fullmove counter (starts at 1, incremented after Black's move)

	};
}