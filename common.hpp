/**  common.hpp
    Structs, exception types, and enums used by UnitTestris classes.
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#include <stdexcept>
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
  // Arbitrary, but useful for stl.
  inline bool operator<(const coord&right) const
  {
    if(this->x<right.x) return true;
    else if (this->x>right.x) return false;
    // a.x==b.x
    return this->y<right.y;;
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

class FieldSizeError : public std::out_of_range
{
public:
  FieldSizeError() : std::out_of_range( "Argument does not match Tetris field dimensions." )
  {
  }
};

class DuplicateBlockError: public std::logic_error
{
public:
  DuplicateBlockError() : std::logic_error( "Block already exists." )
  {
  }
};

class PieceLockError: public std::logic_error
{
public:
  PieceLockError() : std::logic_error("Piece is already locked!")
  {
  }
};

class GameRunningError: public std::runtime_error
{
public:
  GameRunningError() : std::runtime_error("Attempt to call a configuration function during run.")
  {
  }
};

class GameNotRunningError: public std::runtime_error
{
public:
  GameNotRunningError() : std::runtime_error("Attempt to call a control function on a game which is not running.")
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
