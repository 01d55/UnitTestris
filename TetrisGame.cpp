#include "TetrisGame.hpp"

TetrisGame::TetrisGame()
{}
TetrisGame::TetrisGame(IRenderFunc *callback):cb(callback)
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
void TetrisGame::setRenderer(IRenderFunc *callback)
  throw (GameRunningError)
{
  // STUB
}
// Control functions. May be called asyncronously.
void TetrisGame::queueInput(PieceInput) throw (GameNotRunningError)
{
  // STUB
}
