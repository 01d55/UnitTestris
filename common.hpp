/**  common.hpp
    Structs, exception types, and enums used by UnitTestris classes.
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#include <sstream>

#define FIELD_HEIGHT 22
#define FIELD_WIDTH 10
#define FIELD_SIZE FIELD_HEIGHT*FIELD_WIDTH
struct coord
{
  int x;
  int y;

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
  inline const coord& operator=(const coord&right)
  {
    x=right.x;
    y=right.y;
    return *this;
  }
  inline const coord& operator+=(const coord&right)
  {
    x+=right.x;
    y+=right.y;
    return *this;
  }
  inline const coord& operator-=(const coord&right)
  {
    x-=right.x;
    y-=right.y;
    return *this;
  }
  inline coord operator+(const coord&right) const
  {
    return coord(x+right.x,y+right.y);
  }
  inline coord operator-(const coord&right) const
  {
    return coord(x-right.x,y-right.y);
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

// Why is this not in the STL?
template <class T>
inline std::string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}

#endif // COMMON_HPP
