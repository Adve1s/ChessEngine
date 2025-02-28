# **Chess Engine**

*A modern C++ chess engine learning project implementing bitboard-based move generation and representation.*

> 📚 **Learning Project**: This is a personal project for learning chess programming concepts and advanced C++ techniques. Code and documentation are being developed with learning and exploration as primary goals.

## **Current Status**

🚧 **Work in Progress** 🚧

This project is actively under development. Core components for board representation and move generation are implemented, but the engine is not yet fully functional for game play.

## **Implemented Features**

✅ **Efficient Bitboard Representation** - Using 64-bit integers to represent the chess board  
✅ **Magic Bitboards** - Fast sliding piece attack generation  
✅ **Move Encoding** - Compact 16-bit representation for all legal chess moves  
✅ **Board Utilities** - File/rank mapping, square distance calculations, and other core functionality  

## **Planned Features**

🔲 Position evaluation  
🔲 Search algorithms (Minimax with alpha-beta pruning)  
🔲 Opening book support  
🔲 UCI protocol compatibility  
🔲 Transposition tables  
🔲 Multi-threading support  
🔲 End-game tablebases  

## **Architecture**

The engine uses a modern C++ approach with a focus on efficiency and clean code:

- **Bitboards**: Core representation using 64-bit integers where each bit represents a square
- **Magic Bitboards**: Pre-computed lookup tables for fast sliding piece move generation
- **Move Representation**: Compact 16-bit encoding with support for special moves (castling, en passant, promotions)
- **Type Safety**: Strong typing with enums for pieces, squares, and move types

## **Technical Design**

### **Bitboard Implementation**
The engine uses bitboards (64-bit integers) to represent piece positions and attack patterns. This allows for efficient move generation and position evaluation using bitwise operations.

### **Magic Bitboards**
For sliding pieces (bishops, rooks, queens), the engine uses the "magic bitboards" technique - a perfect hashing approach to quickly compute legal moves without iterating through squares.

### **Move Encoding**
Moves are encoded in 16 bits:
- Bits 0-5: Origin square (0-63)
- Bits 6-11: Destination square (0-63)
- Bits 12-13: Promotion piece type (0-3)
- Bits 14-15: Move type (normal, promotion, en passant, castling)

## **Building the Project**

Currently using Visual Studio 2022 with C++20 support. The project requires:
- C++20 compatible compiler
- The GSL (Guidelines Support Library)

More detailed build instructions will be provided when the engine reaches a more complete state.

## **Usage**

```cpp
// Initialize the engine
chess::init();
chess::initMagicBitboards();

// Example move creation
Move m = Move(chess::E2, chess::E4);  // e2-e4
```
## **Inspiration and References**

This project draws inspiration from established open-source chess engines like:
- Stockfish
- Crafty
- Fruit

---

*This project is a learning exercise in chess programming and advanced C++ techniques.*
