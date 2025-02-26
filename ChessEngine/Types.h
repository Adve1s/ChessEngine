#pragma once
#include <iostream>
#include <cstdint>

namespace chess {

	// Define the bitboard type
	using Bitboard = uint64_t;

	// Piece types
	enum PieceType : int
	{
		NO_PIECE_TYPE = 0, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
		ALL_PIECES = 0,
		PIECE_TYPE_NB = 8
	};

	enum Piece : int
	{
		NO_PIECE,      // = 0 (first enum value defaults to 0)

		// White pieces start at PAWN's value (1)
		W_PAWN = PAWN, // = 1
		W_KNIGHT,      // = 2 
		W_BISHOP,      // = 3
		W_ROOK,        // = 4
		W_QUEEN,       // = 5
		W_KING,        // = 6

		// Black pieces start at PAWN + 8 (9)
		B_PAWN = PAWN + 8, // = 9
		B_KNIGHT,          // = 10
		B_BISHOP,          // = 11
		B_ROOK,           // = 12
		B_QUEEN,          // = 13
		B_KING,           // = 14

		PIECE_NB = 16      // Total number of possible pieces
		// Values 7 - 8 and 15 are unused
	};

	// Player colors
	enum Color : int
	{
		WHITE,
		BLACK,
		NO_COLOR = 2,
		COLOR_NB = 2
	};

	// Square identifiers (0-63, plus NO_SQUARE)
	enum Square : int
	{
		A1 = 0, B1, C1, D1, E1, F1, G1, H1,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A8, B8, C8, D8, E8, F8, G8, H8,
		NO_SQUARE = 64,
		SQUARE_ZERO = 0,
		SQUARE_NB = 64
	};

	// Move types
	enum MoveType : int
	{
		NORMAL = 0,             // Regular move
		CAPTURE = 1,            // Capturing an opponent's piece
		PAWN_TWO_FORWARD = 2,   // Pawn's initial two-square move
		KINGSIDE_CASTLE = 3,    // King castling to the kingside
		QUEENSIDE_CASTLE = 4,   // King castling to the queenside
		EN_PASSANT = 5,         // En passant capture
		PROMOTION = 6,          // Pawn promotion
		PROMOTION_CAPTURE = 7,   // Pawn promotion with capture
		MOVE_TYPE_NB = 8
	};

	// Directions for sliding pieces and move generation
	enum Direction : int
	{
		NORTH = 8,
		EAST = 1,
		SOUTH = -8,
		WEST = -1,
		NORTH_EAST = NORTH + EAST,
		NORTH_WEST = NORTH + WEST,
		SOUTH_EAST = SOUTH + EAST,
		SOUTH_WEST = SOUTH + WEST
	};

	// File identifiers (0-7)
	enum File : int
	{
		FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
	};

	// Rank identifiers (0-7)
	enum Rank : int
	{
		RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
	};

	// Constants
	constexpr int MAX_MOVES = 256;         // Maximum number of moves in a position
	constexpr int MAX_GAME_LENGTH = 246;  // Maximum number of half-moves in a game

	// Utility functions
	constexpr bool isSquare(const Square s) {return s >= SQUARE_ZERO && s < NO_SQUARE;}
	constexpr File fileOf(const Square sq) { return static_cast<File>(sq & 7); }
	constexpr Rank rankOf(const Square sq) { return static_cast<Rank>(sq >> 3); }
	constexpr Square makeSquare(const File f, const Rank r) { return static_cast<Square>((r << 3) | f); }

	// Castling rights (can be combined with bitwise OR)
	enum CastlingRights : int
	{
		NO_CASTLING,									// 0000
		WHITE_OO,									    // 0001
		WHITE_OOO = WHITE_OO << 1,						// 0010
		BLACK_OO = WHITE_OO << 2,  						// 0100
		BLACK_OOO = WHITE_OO << 3,						// 1000
			
		KING_SIDE = WHITE_OO | BLACK_OO,		        // 0101
		QUEEN_SIDE = WHITE_OOO | BLACK_OOO,				// 1010
		WHITE_CASTLING = WHITE_OO | WHITE_OOO,			// 0011
		BLACK_CASTLING = BLACK_OO | BLACK_OOO,			// 1100
		ANY_CASTLING = WHITE_CASTLING | BLACK_CASTLING, // 1111

		CASTLING_RIGHT_NB = 16
	};

#define ENABLE_INCR_OPERATORS_ON(T) \
    inline T& operator++(T& d) noexcept  { return d = T(int(d) + 1); } \
    inline T& operator--(T& d) noexcept  { return d = T(int(d) - 1); }

	ENABLE_INCR_OPERATORS_ON(Square)



	// Helper to report test results
	inline void report(const std::string& testName, bool success) {
		std::cout << (success ? "PASS: " : "FAIL: ") << testName << "\n";
	}
}
