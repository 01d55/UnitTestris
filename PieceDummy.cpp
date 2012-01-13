#include "PieceDummy.hpp"
#include "Piece.hpp"

namespace PieceDummy
{
  int timeStep_count=0;
  std::vector<PieceInput> handleInput_args;

  int count_timeStep()
  {
    return timeStep_count;
  }

  bool compare_handleInput_arg(const std::vector<PieceInput> &args)
  {
    return args == handleInput_args;
  }


  void reset_timeStep()
  {
    timeStep_count=0;
  }
  void reset_handleInput()
  {
    handleInput_args.clear();
  }

  void reset_all()
  {
    reset_timeStep();
    reset_handleInput();
  }
}

//public:
Piece::Piece(PieceType t, unsigned int d, Field *f):
  type(t),baseDelay(d),field(f)
{

}

bool Piece::timeStep(unsigned int g) 
{
  // STUB
  ++PieceDummy::timeStep_count;
  return false;
}
bool Piece::handleInput(PieceInput in)
{
  // STUB
  PieceDummy::handleInput_args.push_back(in);
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
