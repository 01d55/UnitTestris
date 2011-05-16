#include "Piece.hpp"

Piece::Piece(PieceType t, const int *d, Field *f)
{
  // STUB
}

bool Piece::timeStep(int t) throw (PieceLockError)
{
  // STUB
  return false;
}

bool Piece::handleInput(PieceInput in) throw (PieceLockError)
{
  // STUB
  return false;
}

coord Piece::getCenter() const
{
  // STUB
  coord ret(-1,-1);
  return ret;
}
std::vector<coord> Piece::getBlocks() const
{
  // STUB
  std::vector<coord> ret;
  return ret;
}
