#ifndef FIELDDUMMY_HPP
#define FIELDDUMMY_HPP
#include <set>
#include <map>

namespace FieldDummy
{
  bool compare_get_arg(std::set<coord> args);
  bool compare_set_arg(std::set<coord> args);
  int count_readScore();
  int count_resetScore();
  int count_resetBlocks();

  void populate_get_results(coord arg, bool result);
  void reset_readScore();
  void reset_resetScore();
  void reset_resetBlocks();
}

#endif // FIELDDUMMY_HPP
