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
  type(t),baseDelay(d),lockDelay(d),field(f),center(4,20),lock(false)
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
  if(lock)
    {throw PieceLockError();}

  for(int i=g;i>0;--i)
    {
      if(can_drop())
	{
	  --center.y;
	  if(!can_drop())
	    {
	      if(0==lockDelay)
		{
		  invoke_lock();
		  break;
		}
	      else
		{
		  --lockDelay;
		}
	    }
	}
      else
	{
	  if(0==lockDelay)
	    {
	      invoke_lock();
	      break;
	    }
	  else
	    {
	      --lockDelay;
	    }
	}
    }
  return lock;
}

bool Piece::handleInput(PieceInput in) throw (PieceLockError)
{
  // STUB
  if(lock)
    {
      throw PieceLockError();
    }

  switch (in)
    {
    case shift_right:
      break;
    case shift_left:
      break;
    case rotate_cw:
      break;
    case rotate_ccw:
      break;
    case hard_drop:
      timeStep(22+lockDelay);
      break;
    default:
      ;
    }
  return lock;
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

// Private functions
/* Return true if the Piece can shift down 1.
 */
bool Piece::can_drop() const
{
  boost::array<coord,4>blocks=getBlocks();
  
  for(int i=0;i<4;++i)
    {
      --blocks[i].y;
      try
	{
	  if( field->get(blocks[i]) ) 
	    {
	      return false;
	    }
	}
      catch(FieldSizeError)
	{
	  return false;
	}
    }
  return true;
}
/* Return true if every co-ord in blocks is in the field and
   is not filled.
*/
bool Piece::can_place(boost::array<coord,4> blocks) const
{
  for(int i=0;i<4;++i)
    {
      try
	{
	  if(field->get(blocks[i]))
	    {
	      return false;
	    }
	}
      catch(FieldSizeError)
	{
	  return false;
	}
    }
  return true;
}
// Put blocks in the field and lock self.
void Piece::invoke_lock()
{
  boost::array<coord,4>blocks=getBlocks();
  for(int i=0;i<4;++i)
    {
      field->set(blocks[i]);
    }
  lock=true;
}
