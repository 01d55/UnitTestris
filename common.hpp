/**  common.hpp
    Structs, exception types, and enums used by UnitTestris classes.
 */

#ifndef COMMON_HPP
#define COMMON_HPP


struct coord
{
  int x;
  int y;
};

enum PieceType
  {
    I,J,L,O,S,T,Z
  };
enum PieceInput
  {
    shift_right,
    shift_left,
    rotate_cw,
    rotate_ccw,
    soft_drop,
    hard_drop
  };

class FieldSizeError : public std::runtime_error
{
public:
  FieldSizeError() : runtime_error( "Argument does not match Tetris field dimensions." )
  {
  }
};

class DuplicateBlockError: public std::runtime_error
{
public:
  DuplicateBlockError() : runtime_error( "Block already exists." )
  {
  }
};

class PieceLockError: public std::runtime_error
{
public:
  PieceLockError() : std::runtime_error("Piece is already locked!")
  {
  }
};


#endif // COMMON_HPP
