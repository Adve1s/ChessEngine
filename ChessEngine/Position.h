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

	struct StateInfo {
		// Hash keys for various aspects of the position
		HashKey positionKey;    // Full position hash
		HashKey materialKey;    // Material configuration hash
		HashKey pawnKey;        // Pawn structure hash

		// Material counting
		std::array<Value, COLOR_NB> nonPawnMaterial;  // Total value of non-pawn pieces by color

		// Check and pin information
		Bitboard checkersBB;                // Pieces giving check
		std::array <Bitboard, COLOR_NB> blockersForKing; // Pieces blocking attacks to kings
		std::array <Bitboard, COLOR_NB> pinners;         // Enemy pieces pinning friendly pieces

		// Game state variables
		Color activeColor;           // Side to move
		CastlingRights castlingRights; // Current castling availability
		Square epSquare;             // En passant target square
		int halfmoveClock;           // Halfmove clock for 50-move rule
		int fullmoveNumber;          // Fullmove counter

		// Previous move information
		Piece capturedPiece;         // Piece captured in the last move
		int repetition;              // Position repetition counter

		// Linked list pointers
		StateInfo* previous;

		// Constructor declaration
		StateInfo() noexcept;
	};

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
		StateInfo* m_state;
		StateInfo m_startState;

	};
}