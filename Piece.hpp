#ifndef PIECE_HPP
#define PIECE_HPP

#include <stdexcept>
#include <vector>
#include "common.hpp"
#include "Field.hpp"

/* Piece
   The piece class enforces the following rules of Tetris: The piece enters play
   in the two rows immediately above the top visible row of the field. Its 
   orientation and position are as described in SRS, with the exception that 
   there are no wall kicks.

   The piece relies on the game loop to call timeStep once per frame, if g is 
   greater than or equal to 1, or once per 1/g frames, if g is less than one. 
   g is a parameter to the timeStep function so that the game can change g 
   while the piece is in play. The lock delay is defined in an external 
   reference and may be zero. Either timeStep or a hard drop may cause a piece 
   to lock, which causes the piece to place its pieces into the Field. When 
   this happens the function that caused the piece to lock will return true; 
   otherwise these functions return false. Attempting to timeStep or move on a 
   locked piece is an error.

   The piece tracks it's location in the field through its axis of rotation as 
   defined by SRS. The I and O blocks rotate not around one of their 
   constituent blocks, but a point between block spaces. In the case of the I 
   block, it is possible for the point of rotation to be placed against the 
   sides of the field. Therefore, the I block's center of rotation has 11, not 
   10, possible x co-ordinates (and hypothetically, 23 possible y co-ordinates.)
 */




class Piece
{
public:
  Piece(PieceType t, const int *d, Field *f);

  bool timeStep(int g) throw (PieceLockError);
  bool handleInput(PieceInput in) throw (PieceLockError);

  coord getCenter() const;
  std::vector<coord> getBlocks() const;

private:
};

#endif // PIECE_HPP
