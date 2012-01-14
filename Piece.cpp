#include "Piece.hpp"



static const coord originI(5,20);
static const coord originO(5,21);
// Initial values of relative blocks, by type
static const arrayt relative_I = {
  { coord(-2,0),coord(-1,0),coord(0,0),coord(1,0) }
};
static const arrayt relative_O = {
  { coord(-1,-1),coord(-1,0),coord(0,-1),coord(0,0) }
};
static const arrayt relative_L = {
  { coord(-1,0),coord(-0,0),coord(1,0),coord(1,1) }
};
static const arrayt relative_J = {
  { coord(-1,0),coord(-1,1),coord(0,0),coord(1,0) }
};
static const arrayt relative_S = {
  { coord(-1,0),coord(0,1),coord(0,0),coord(1,1) }
};
static const arrayt relative_Z = {
  { coord(-1,1),coord(0,1),coord(0,0),coord(1,0) }
};
static const arrayt relative_T = {
  { coord(-1,0),coord(0,1),coord(0,0),coord(1,0) }
};

inline coord cw(const coord &a)
{
  return(coord(a.y,-a.x));
}

inline coord ccw(const coord &a)
{
  return(coord(-a.y,a.x));
}

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

bool Piece::timeStep(unsigned int g)
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

bool Piece::handleInput(PieceInput in)
{

  if(lock)
    {
      throw PieceLockError();
    }

  switch (in)
    {
    case shift_right:
      if( can_shift(coord(1,0)) )
	{
	  ++center.x;
	}
      break;
    case shift_left:
      if( can_shift(coord(-1,0)) )
	{
	  --center.x;
	}
      break;
    case rotate_cw:
    case rotate_ccw:
      rotate(in);
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
arrayt Piece::getBlocks() const
{
  arrayt ret=relative_blocks;
  for(int i=0;i<4;++i)
    {
      ret[i]+=center;
    }
  return ret;
}

// Private functions
/* Return true if the Piece's center can shift by displacement without leaving
 * the field or intersecting existing blocks.
 */
bool Piece::can_shift(const coord &displacement) const
{
  arrayt blocks=getBlocks();
  
  for(int i=0;i<4;++i)
    {
      blocks[i]+=displacement;
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
bool Piece::can_place(arrayt blocks) const
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

// Rotate the relative blocks
void Piece::rotate(PieceInput in)
{
  arrayt rblocks, blocks;
  int i;
  // Sanity check: this should never happen!
  if( in!=rotate_ccw && in!=rotate_cw)
    {
      throw in;
    }

  if(type==O)
    {
      return;
    }
  rblocks=relative_blocks;
  if(type==I)
    {
      /*
       *__________*     *__________*
       *          * 2   *          * 2
       *          * 1   *   II II  * 1
       *   IIcI   * 0   *     c    * 0
       *          * 1   *          * 1
       *          * 2   *          * 2
       *5432101234*     *5432101234*
       1 rotation
       *__________*     *__________*
       *          * 2   *    I     * 2
       *    I     * 1   *    I     * 1
       *    Ic    * 0   *     c    * 0
       *    I     * 1   *    I     * 1 
       *    I     * 2   *    I     * 2
       *5432101234*     *5432101234*
       2 rotations
       *__________*     *__________*
       *          * 2   *          * 2
       *          * 1   *          * 1
       *     c    * 0   *     c    * 0
       *   IIII   * 1   *   II II  * 1
       *          * 2   *          * 2
       *5432101234*     *5432101234*
       3 rotations
       *__________*     *__________*  
       *          * 2   *      I   * 2
       *     I    * 1   *      I   * 1
       *     c    * 0   *     c    * 0
       *     I    * 1   *      I   * 1
       *     I    * 2   *      I   * 2
       *5432101234*     *5432101234*
       */
      for(i-0;i<4;++i)
	{
	  // Shift all nonnegative co-ordinates up by 1
	  if(rblocks[i].x >= 0)
	    {
	      ++rblocks[i].x;
	    }
	  if(rblocks[i].y >= 0)
	    {
	      ++rblocks[i].y;
	    }
	  // rotate
	  rblocks[i]= (in==rotate_cw) ? cw(rblocks[i]) : ccw(rblocks[i]);
	  // un-shift
	  if(rblocks[i].x > 0)
	    {
	      --rblocks[i].x;
	    }
	  if(rblocks[i].y > 0)
	    {
	      --rblocks[i].y;
	    }
	  blocks[i]=rblocks[i]+center;
	}
      if(can_place(blocks))
	{
	  relative_blocks=rblocks;
	}
    }
  else 
    {
      for(i=0;i<4;++i)
	{
	  rblocks[i]= (in==rotate_cw) ? cw(rblocks[i]) : ccw(rblocks[i]);
	  blocks[i]=rblocks[i]+center;
	}
      if(can_place(blocks))
	{
	  relative_blocks=rblocks;
	}
    }
}


// Put blocks in the field and lock self.
void Piece::invoke_lock()
{
  arrayt blocks=getBlocks();
  for(int i=0;i<4;++i)
    {
      field->set(blocks[i]);
    }
  lock=true;
}
