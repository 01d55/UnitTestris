#include "Piece.hpp"



static const coord originI(5,20);
static const coord originO(5,21);
// Initial values of relative blocks, by type
static const boost::array <coord,4> relative_I = {
  { coord(-2,0),coord(-1,0),coord(0,0),coord(1,0) }
};
static const boost::array <coord,4> relative_O = {
  { coord(-1,-1),coord(-1,0),coord(0,-1),coord(0,0) }
};
static const boost::array <coord,4> relative_L = {
  { coord(-1,0),coord(-0,0),coord(1,0),coord(1,1) }
};
static const boost::array <coord,4> relative_J = {
  { coord(-1,0),coord(-1,1),coord(0,0),coord(1,0) }
};
static const boost::array <coord,4> relative_S = {
  { coord(-1,0),coord(0,1),coord(0,0),coord(1,1) }
};
static const boost::array <coord,4> relative_Z = {
  { coord(-1,1),coord(0,1),coord(0,0),coord(1,0) }
};
static const boost::array <coord,4> relative_T = {
  { coord(-1,0),coord(0,1),coord(0,0),coord(1,0) }
};


Piece::Piece(PieceType t, unsigned int d, Field *f):
  type(t),lockDelay(d),field(f),center(4,20)
{

  switch (t)
    {
    case I:
      center=originI;
      relative_blocks=relative_I;
      break;
    case O:
      center=originO;
      relative_blocks=relative_O;
      break;
    case L:
      relative_blocks=relative_L;
      break;
    case J:
      relative_blocks=relative_J;
      break;
    case S:
      relative_blocks=relative_S;
      break;
    case Z:
      relative_blocks=relative_Z;
      break;
    case T:
      relative_blocks=relative_T;
      break;
    default:
      ;
    }
}

bool Piece::timeStep(unsigned int g) throw (PieceLockError)
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
  return center;
}
boost::array<coord,4> Piece::getBlocks() const
{
  boost::array<coord,4> ret=relative_blocks;
  for(int i=0;i<4;++i)
    {
      ret[i]+=center;
    }
  return ret;
}
