#ifndef TETRISGAME_HPP
#define TETRISGAME_HPP

#include "RenderFunc.hpp"
#include "common.hpp"
#include <memory>
/* TetrisGame
   A tetris game, running on its own thread. Attepts to execute the core loop once each 
   1/60th of a second.

   The core loop will read and execute all available input before incrementing 
   frame-based counters (e.g. gravity, lock delay). At the end of each loop, the 
   callback set by setRenderer will be called with read-only references to the Field, 
   current Piece, and hold Piece.

   Neither TetrisGame nor the templated implementation of IRenderFunc synchronize with
   other threads - the callback will be executed from the TetrisGame's thread and it is
   the responsibility of that callback to aquire appropriate locks before modifying
   client data.
 */
struct TetrisGame_impl;
class TetrisGame
{
  friend struct TetrisGame_impl;
public:
  TetrisGame();

  TetrisGame(IRenderFunc *callback);
  ~TetrisGame();

  // Start/stop the thread.
  void run(); //throw(GameRunningError);
  void pause(); //throw(GameNotRunningError);
  // Render callback. Calling during run is an error.
  // Piece reference is the current piece, the piece pointer is the "hold" piece
  // The "hold" piece will be NULL if there is no "hold" piece.
  void setRenderer( IRenderFunc* callback ); // throw (GameRunningError);
  // May be called only while the game is running. The internal thread will aquire a 
  // lock and consume the entire queue once each frame, processing each input in the
  // order it was recieved. 
  void queueInput(PieceInput in); //throw (GameNotRunningError);
  // Read whether the game has ended. If the game is over, there will be no
  // further callbacks.
  bool isGameOver();
protected:
private:
  TetrisGame(const TetrisGame&) = delete; // Uncopyable
  std::unique_ptr<TetrisGame_impl> me;
};
#endif // TETRISGAME_HPP
