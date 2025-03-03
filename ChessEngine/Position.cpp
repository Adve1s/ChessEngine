#include "Position.h"

#include <random>

#include "BitBoard.h"

// Position.cpp - Chess position representation and manipulation

//---------------------------------------------------------------
// Performance: Disable array bounds checking warnings (26446)
// Using direct array access with fixed enum indices (WHITE=0, etc.)
// This is safe due to fixed array sizes matching enum ranges,
// and critical for chess engine performance
//---------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 26446)
#pragma warning(disable: 26482)

namespace chess {

	// Declare zobrist arrays
	namespace zobrist {
		std::array<std::array<HashKey, SQUARE_NB>, PIECE_NB> g_pieceSq;		// Piece-square keys
		std::array<HashKey, FILE_NB> g_enpassant;							// En passant keys
		std::array<HashKey, CASTLING_RIGHT_NB> g_castling;					// Castling rights keys
		HashKey g_side;														// Side to move key
		HashKey g_noPawns;													// No pawns key
	}

	StateInfo::StateInfo() noexcept:
		positionKey(0),
		materialKey(0),
		pawnKey(0),
		checkersBB(0),
		activeColor(WHITE),
		castlingRights(NO_CASTLING),
		epSquare(NO_SQUARE),
		halfmoveClock(0),
		fullmoveNumber(1),
		capturedPiece(NO_PIECE),
		repetition(0),
		previous(nullptr)
	{
		nonPawnMaterial[WHITE] = nonPawnMaterial[BLACK] = 0;
		blockersForKing[WHITE] = blockersForKing[BLACK] = 0;
		pinners[WHITE] = pinners[BLACK] = 0;
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
		// Clear the board representation
		std::fill(m_board.begin(), m_board.end(), NO_PIECE);
		std::fill(m_pieceBB.begin(), m_pieceBB.end(), Bitboard{0});
		std::fill(m_colorBB.begin(), m_colorBB.end(), Bitboard{0});
		std::fill(m_pieceCount.begin(), m_pieceCount.end(), 0);

		// Clear castling data
		std::fill(m_castlingRightsMask.begin(), m_castlingRightsMask.end(), 0);
		std::fill(m_castlingRookSquare.begin(), m_castlingRookSquare.end(), NO_SQUARE);
		std::fill(m_castlingPath.begin(), m_castlingPath.end(), Bitboard{0});

		// Reset state to a new StateInfo
		m_state = &m_startState;

		// Initialize StateInfo with default values
		m_state->positionKey = 0;
		m_state->materialKey = 0;
		m_state->pawnKey = 0;
		m_state->nonPawnMaterial[WHITE] = m_state->nonPawnMaterial[BLACK] = 0;
		m_state->checkersBB = 0;
		m_state->blockersForKing[WHITE] = m_state->blockersForKing[BLACK] = 0;
		m_state->pinners[WHITE] = m_state->pinners[BLACK] = 0;
		m_state->activeColor = WHITE;
		m_state->castlingRights = NO_CASTLING;
		m_state->epSquare = NO_SQUARE;
		m_state->halfmoveClock = 0;
		m_state->fullmoveNumber = 1;
		m_state->capturedPiece = NO_PIECE;
		m_state->repetition = 0;
		m_state->previous = nullptr;
	}

	void Position::putPiece(Piece piece, Square square) {
		assert(isSquare(square));
		assert(piece != NO_PIECE);
		assert(m_board[square] == NO_PIECE);  // Square should be empty

		// Update piece array
		m_board[square] = piece;

		// Update bitboards
		m_pieceBB[ALL_PIECES] |= squareToBB(square);
		m_pieceBB[typeOf(piece)] |= squareToBB(square);
		m_colorBB[colorOf(piece)] |= squareToBB(square);

		// Update piece count
		++m_pieceCount[piece];
	}

	void Position::removePiece(Square square) {
		assert(isSquare(square));
		assert(m_board[square] != NO_PIECE);  // Square should not be empty

		const Piece piece = m_board[square];

		// Update piece array
		m_board[square] = NO_PIECE;

		// Update bitboards
		m_pieceBB[ALL_PIECES] &= ~squareToBB(square);
		m_pieceBB[typeOf(piece)] &= ~squareToBB(square);
		m_colorBB[colorOf(piece)] &= ~squareToBB(square);

		// Update piece count
		--m_pieceCount[piece];
	}

	void Position::movePiece(Square from, Square to) {
		assert(isSquare(from) && isSquare(to));
		assert(m_board[from] != NO_PIECE);  // Source square should not be empty
		assert(m_board[to] == NO_PIECE);    // Destination square should be empty

		Piece piece = m_board[from];
		Bitboard fromToBB = squareToBB(from) | squareToBB(to);

		// Update piece array
		m_board[from] = NO_PIECE;
		m_board[to] = piece;

		// Update bitboards (flipping both bits with XOR is more efficient)
		m_pieceBB[ALL_PIECES] ^= fromToBB;
		m_pieceBB[typeOf(piece)] ^= fromToBB;
		m_colorBB[colorOf(piece)] ^= fromToBB;
	}
}
#pragma warning(pop)