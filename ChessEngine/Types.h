#pragma once
#include <iostream>
#include <cstdint>
#include <cassert>

// Types.h - Core types and constants for the chess engine

namespace chess {

	// 64-bit unsigned integer representing a chess board
	using Bitboard = uint64_t;

	// 64-bit hash value for positions
	using HashKey = uint64_t;

	// Piece types
	enum PieceType : int
	{
		NO_PIECE_TYPE = 0,
		PAWN = 1,
		KNIGHT = 2,
		BISHOP = 3,
		ROOK = 4,
		QUEEN = 5,
		KING = 6,
		ALL_PIECES = 0,
		PIECE_TYPE_NB = 8
	};

	// Pieces (combines piece type and color)
	enum Piece : int
	{
		NO_PIECE = 0,      // = 0 (first enum value defaults to 0)

		// White pieces start at PAWN's value (1)
		W_PAWN = PAWN,		// = 1
		W_KNIGHT = KNIGHT,  // = 2 
		W_BISHOP = BISHOP,  // = 3
		W_ROOK = ROOK,      // = 4
		W_QUEEN = QUEEN,    // = 5
		W_KING = KING,      // = 6

		// Black pieces start at PAWN + 8 (9)
		B_PAWN = PAWN + 8,		// = 9
		B_KNIGHT = KNIGHT + 8,  // = 10
		B_BISHOP = BISHOP + 8,  // = 11
		B_ROOK = ROOK + 8,      // = 12
		B_QUEEN = QUEEN + 8,    // = 13
		B_KING = KING + 8,      // = 14

		PIECE_NB = 16      // Total number of possible pieces
		// Values 7 - 8 and 15 are unused
	};

	// Player colors
	enum Color : int
	{
		WHITE = 0,
		BLACK = 1,
		NO_COLOR = 2,
		COLOR_NB = 2
	};

	// Square identifiers (0-63, plus NO_SQUARE)
	enum Square : int
	{
		A1 = 0,  B1 = 1,  C1 = 2,  D1 = 3,  E1 = 4,  F1 = 5,  G1 = 6,  H1 = 7,
		A2 = 8,  B2 = 9,  C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15,
		A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23,
		A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31,
		A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39,
		A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47,
		A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55,
		A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63,
		NO_SQUARE = 64,
		SQUARE_ZERO = 0,
		SQUARE_NB = 64
	};

	// Move types
	enum MoveType : int {
		NORMAL = 0,
		PROMOTION = 1 << 14,
		EN_PASSANT = 2 << 14,
		CASTLING = 3 << 14
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

	// Castling rights (can be combined with bitwise OR)
	enum CastlingRights : int
	{
		NO_CASTLING = 0,								// 0000
		WHITE_OO = 1,									// 0001
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

	// Constants
	constexpr int MAX_MOVES = 256;         // Maximum number of moves in a position
	constexpr int MAX_GAME_LENGTH = 246;  // Maximum number of half-moves in a game

	using Value = int;  // Type alias for evaluation and material values

	// Material values for pieces
	constexpr Value PawnValue = 208;
	constexpr Value KnightValue = 781;
	constexpr Value BishopValue = 825;
	constexpr Value RookValue = 1276;
	constexpr Value QueenValue = 2538;
	// We don't need a specific value for the king in material counting

	// Special evaluation values
	constexpr Value VALUE_ZERO = 0;
	constexpr Value VALUE_DRAW = 0;
	constexpr Value VALUE_NONE = 32002;
	constexpr Value VALUE_INFINITE = 32001;
	constexpr Value VALUE_MATE = 32000;
	constexpr Value VALUE_MATE_IN_MAX_PLY = VALUE_MATE - MAX_GAME_LENGTH;
	constexpr Value VALUE_MATED_IN_MAX_PLY = -VALUE_MATE_IN_MAX_PLY;

	// Utility functions
	constexpr bool isSquare(const Square s) { return s >= SQUARE_ZERO && s < NO_SQUARE;}
	constexpr File fileOf(const Square sq) { assert(isSquare(sq)); return static_cast<File>(sq & 7); }
	constexpr Rank rankOf(const Square sq) { assert(isSquare(sq)); return static_cast<Rank>(sq >> 3); }
	constexpr Square makeSquare(const File f, const Rank r) { assert(f < FILE_NB && r < RANK_NB); return static_cast<Square>((r << 3) | f); }
	constexpr PieceType typeOf(const Piece piece) {return static_cast<PieceType>(piece & 7);}
	constexpr Color colorOf(const Piece piece) {assert(piece != NO_PIECE); return static_cast<Color>(piece >> 3);
	}

	// Operator overload
	inline Square operator+(const Square sq,const Direction dir) noexcept{
		return static_cast<Square>(static_cast<int>(sq) + static_cast<int>(dir));
	}

	// Operator overloads for enum types
	template<typename T>
	T& operator++(T& d) noexcept {
		return d = static_cast<T>(static_cast<int>(d) + 1);
	}

	template<typename T>
	T& operator--(T& d) noexcept {
		return d = static_cast<T>(static_cast<int>(d) - 1);
	}

	// Explicit specializations for Square
	template<> inline Square& operator++(Square& d) noexcept {
		return d = static_cast<Square>(static_cast<int>(d) + 1);
	}

	template<> inline Square& operator--(Square& d) noexcept {
		return d = static_cast<Square>(static_cast<int>(d) - 1);
	}

	// Helper to report test results
	inline void report(const std::string& testName, bool success) {
		std::cout << (success ? "PASS: " : "FAIL: ") << testName << "\n";
	}
}
