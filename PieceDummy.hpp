#ifndef PIECEDUMMY_HPP
#define PIECEDUMMY_HPP

#include <vector>

#include "common.hpp"
#include "config.h"

namespace PieceDummy
{
  // STUB

  int count_timeStep();
  bool compare_handleInput_arg(const std::vector<PieceInput> &args);

  void reset_timeStep();
  void reset_handleInput();
  void reset_all();
}

#endif // PIECEDUMMY_HPP
