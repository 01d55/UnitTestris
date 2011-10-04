#ifndef TETRISGAME_HPP
#define TETRISGAME_HPP

#include "Piece.hpp"

/* TetrisGame
   A tetris game, running on its own thread. Attepts to execute the core loop once each 1/60th of 
   a second. Each loop, will read and execute one input before incrementing frame-based counters 
   (e.g. gravity, lock delay). At the end of each loop, the callback set by setRenderer will be 
   called with read-only references to the Field, current Piece, and hold Piece.
 */
class TetrisGame
{
public:
  TetrisGame();
  TetrisGame(void (*renderFunc)(const Field &, const Piece &, const Piece * ) );
  ~TetrisGame();

  // Start/stop the thread.
  void run() throw(GameRunningError);
  void pause() throw(GameNotRunningError);
  // Render callback. Calling during run is an error.
  // Piece reference is the current piece, the piece pointer is the "hold" piece
  // The "hold" piece will be NULL if there is no "hold" piece.
  void setRenderer(void (*renderFunc)(const Field &, const Piece &, const Piece * ) ) 
    throw (GameRunningError);
  // May be called only while the game is running. If the queue has multiple inputs,
  void queueInput(PieceInput) throw (GameNotRunningError);
protected:
private:
  void (*renderCallback)(const Field &, const Piece &, const Piece * );
};
#endif // TETRISGAME_HPP
