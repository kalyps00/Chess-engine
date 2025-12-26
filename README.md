# Chess Engine (C++ / SFML)

A chess engine project written in modern C++17, utilizing **Bitboard** representation for maximum performance and the **SFML** library for graphical visualization. The project is built upon the **MVC (Model-View-Controller)** architectural pattern.

## 🚀 Features

- **Board Representation:** Based on 64-bit integers (`Bitboard` / `uint64_t`), allowing for lightning-fast logical operations.
- **Move Generator:**
  - Pseudo-legal move generation for all pieces (Pawns, Knights, Bishops, Rooks, Queens, King).
  - **En Passant** support.
  - Pawn promotion support.
  - Utilizes `__builtin_ctzll` CPU instruction for fast bit scanning.
- **Graphical Interface (GUI):**
  - Renders board and pieces using SFML.
  - Drag & Drop support.
  - Highlights possible moves for the selected piece.
  - Move validation (prevents executing moves inconsistent with generation rules).

## 🛠️ Architecture (MVC)

The project is divided into three main modules:

### 1. Model (`src/model`)

Responsible for chess logic and game state storage. Independent of the graphics library.

- **`Board`**: Stores game state on 12 bitboards (6 per color) + helper arrays. Contains move execution logic (`make_move`) and En Passant handling.
- **`MoveGenerator`**: Static class generating a vector of available moves (`std::vector<Move>`) based on the current board state.
- **`Types.hpp`**: Type definitions (`Bitboard`), enums (pieces, colors), and the `Move` structure.

### 2. View (`src/view`)

Responsible solely for display.

- **`BoardRender`**: Class using SFML to draw the board, pieces, and highlights. Converts logical coordinates (0-63) to pixels.

### 3. Controller (`src/controller`)

Connects the Model with the View and handles user input.

- **`GameControler`**: Main game loop. Handles mouse events (click, drag), requests move generation, and updates the model.

## 📋 Requirements

- **C++ Compiler**: Supporting C++17 standard (GCC, Clang, MSVC).
- **CMake**: Version 3.16 or newer.
- **SFML Library**: Version 2.5+.

## Build & Run

### Linux / macOS

```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure project (Debug or Release)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 3. Build
cmake --build .

# 4. Run
./ChessEngine
```

## 🗂️ Project Structure

```
Chess-engine/
├── assets/                 # Graphics (pieces, textures)
├── src/
│   ├── controller/         # Control logic (GameControler)
│   ├── model/              # Game logic (Board, MoveGenerator, Bitboards)
│   ├── view/               # Rendering (BoardRender)
│   └── main.cpp            # Entry point
├── CMakeLists.txt          # Build configuration
└── README.md               # Documentation
```

## 📝 Status and Plans (To-Do)

- [x] Bitboard Representation
- [x] Basic Move Generator
- [x] GUI and Mouse Handling
- [x] En Passant
- [x] Castling
- [x] Check and Checkmate Detection
- [x] Choosing promotion piece
- [x] "Game Over" Logic (Checkmate, Stalemate, 50-move rule, Insufficient Material)
- [x] Loading position from FEN
- [ ] 3 move rule
- [ ] **Threefold Repetition** (Requires Zobrist Hashing)
- [ ] **Search Algorithm** (Minimax / Negamax with Alpha-Beta Pruning)
- [ ] **Evaluation Function** (Material + Positional heuristics)
- [ ] UCI Protocol (Universal Chess Interface)
- [ ] Choosing playing side / Restart Game

## ⚡ Optimizations to do

- [ ] **Zobrist Hashing** (Fast position comparison & Transposition Table key)
- [ ] **Transposition Table** (Caching search results)
- [ ] **Move Ordering** (MVV-LVA, Killer Moves, History Heuristic)
- [ ] **Magic Bitboards** (Faster sliding piece attack generation)
- [ ] **Quiescence Search** (Solving horizon effect in tactical positions)
- [ ] **Iterative Deepening** (Time management)
- [ ] Incremental Updates
