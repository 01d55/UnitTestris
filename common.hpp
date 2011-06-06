/**  common.hpp
    Structs, exception types, and enums used by UnitTestris classes.
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#define FIELD_HEIGHT 22
#define FIELD_WIDTH 10
#define FIELD_SIZE FIELD_HEIGHT*FIELD_WIDTH
struct coord
{
  unsigned int x;
  unsigned int y;

  coord():x(0),y(0) {}
  coord (int iX, int iY):x(iX),y(iY)
  {}
  inline bool operator==(const coord&right) const
  {
    return x==right.x && y==right.y;
  }
  inline bool operator!=(const coord&right) const
  {
    return x!=right.x || y!=right.y;
  }
  inline const coord& operator()(int iX, int iY)
  {
    x=iX;
    y=iY;
    return *this;
  }
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
