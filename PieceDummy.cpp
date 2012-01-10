#include "PieceDummy.hpp"
#include "Piece.hpp"


//public:
Piece::Piece(PieceType t, unsigned int d, Field *f):
  type(t),baseDelay(d),field(f)
{

}

bool Piece::timeStep(unsigned int g) 
{
  // STUB
  return false;
}
bool Piece::handleInput(PieceInput in)
{
  // STUB
  return false;
}

coord Piece::getCenter() const
{
  // STUB
  return coord();
}
boost::array<coord,4> Piece::getBlocks() const
{
  // STUB
  return boost::array<coord,4>();
}

//private:
bool Piece::can_shift (const coord &displacement) const
{
  // STUB
  return false;
}
bool Piece::can_place(boost::array<coord,4>) const
{
  // STUB
  return false;
}
void Piece::rotate(PieceInput in)
{
  // STUB
}

void Piece::invoke_lock()
{
  // STUB
}


namespace PieceDummy
{
}
