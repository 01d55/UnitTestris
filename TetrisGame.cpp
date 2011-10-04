#include "TetrisGame.hpp"

TetrisGame::TetrisGame()
{}
TetrisGame::TetrisGame(void (*renderFunc)(const Field &, const Piece &, const Piece * ) )
{}
TetrisGame::~TetrisGame()
{}

// Start the thread.
void TetrisGame::run() throw (GameRunningError)
{
  // STUB
}
// Stop the thread, preserving current state.
void TetrisGame::pause() throw(GameNotRunningError)
{
  // STUB
}
// Render callback. Calling during run is an error.
void TetrisGame::setRenderer(void (*renderFunc)(const Field &, const Piece &, const Piece * ) ) 
  throw (GameRunningError)
{
  // STUB
}
// Control functions. May be called asyncronously.
void TetrisGame::queueInput(PieceInput) throw (GameNotRunningError)
{
  // STUB
}
